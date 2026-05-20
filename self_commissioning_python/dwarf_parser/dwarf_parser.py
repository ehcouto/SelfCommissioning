## @file dwarf_parser.py
# 
# @details    Some useful documentation links:
#             - https://docs.microsoft.com/en-us/windows/win32/debug/pe-format
#        
# @author Leonardo Ricupero

__version__ = '0.4.0'

import copy
import argparse
import sys
import logging
import time
import enum
import xml.etree.ElementTree as ET
import struct
import io

from elftools.elf.elffile import ELFFile
from elftools.dwarf.dwarfinfo import DWARFInfo, DwarfConfig, DebugSectionDescriptor
import pefile


logging.basicConfig()
logger = logging.getLogger(__name__)
logger.setLevel(logging.ERROR)

def get_dwinfo_from_shared_lib(shared_lib_path):
    pe = pefile.PE(shared_lib_path)

    symbol_table_ptr = pe.FILE_HEADER.PointerToSymbolTable
    n_of_symbols = pe.FILE_HEADER.NumberOfSymbols
    
    # parse the string table
    string_table_ptr = symbol_table_ptr + n_of_symbols*18
    string_table_size = struct.unpack('<I', pe.__data__[string_table_ptr:string_table_ptr+4])[0]
    string_table = pe.__data__[string_table_ptr: string_table_ptr + string_table_size]
    s = pe.get_resources_strings()
    
    for section in pe.sections:
        if section.Name.startswith(b"/"):
            # get the real name from the string table
            indx = int(section.Name.decode().strip("\x00")[1:])
            s = string_table[indx:].decode(errors='ignore')
            s_end = s.find("\x00")
            realSectionName = s[:s_end]
            section.Name = realSectionName
        #logger.debug(section.Name, hex(section.VirtualAddress),hex(section.Misc_VirtualSize), section.SizeOfRawData)
    
    debug_info_sec = None
    debug_aranges_sec = None
    debug_abbrev_sec = None
    debug_frame_sec = None
    eh_frame_sec = None
    debug_str_sec = None
    debug_loc_sec = None
    debug_line_sec = None
    debug_ranges_sec = None
    
    for section in pe.sections:
        if section.Name == '.debug_info':
            debug_info_sec = DebugSectionDescriptor(io.BytesIO(section.get_data()), section.Name, 0, section.Misc_VirtualSize)
        elif section.Name == '.debug_aranges':
            debug_aranges_sec = DebugSectionDescriptor(io.BytesIO(section.get_data()), section.Name, 0, section.Misc_VirtualSize)
        elif section.Name == '.debug_abbrev':
            debug_abbrev_sec = DebugSectionDescriptor(io.BytesIO(section.get_data()), section.Name, 0, section.Misc_VirtualSize)
        elif section.Name == '.debug_frame':
            debug_frame_sec = DebugSectionDescriptor(io.BytesIO(section.get_data()), section.Name, 0, section.Misc_VirtualSize)
        elif section.Name == '.debug_ranges':
            debug_ranges_sec = DebugSectionDescriptor(io.BytesIO(section.get_data()), section.Name, 0, section.Misc_VirtualSize)
        elif section.Name == '.debug_str':
            debug_str_sec = DebugSectionDescriptor(io.BytesIO(section.get_data()), section.Name, 0, section.Misc_VirtualSize)
        elif section.Name == '.debug_loc':
            debug_loc_sec = DebugSectionDescriptor(io.BytesIO(section.get_data()), section.Name, 0, section.Misc_VirtualSize)
        elif section.Name == '.debug_line':
            debug_line_sec = DebugSectionDescriptor(io.BytesIO(section.get_data()), section.Name, 0, section.Misc_VirtualSize)
    
    config = DwarfConfig(little_endian=True, 
                         machine_arch = 'x64',
                         default_address_size = 8)
    
    dw_info = DWARFInfo(config, debug_info_sec, debug_aranges_sec, debug_abbrev_sec, debug_frame_sec, eh_frame_sec, debug_str_sec, debug_loc_sec, debug_ranges_sec, debug_line_sec)
    return dw_info

## Reads variables info from a DWARF file
#
# Instance an object of this class in order to create a map of
# the symbols parsed from the ELF file provided. It is then possible
# to refer to the address, type, size and value of the symbols 
# (variables and structure fields)
# in a C fashion (ex: symbols.sBackup.sBlackBox.uw16lastWarning.address) contains
# the address of the variable uw16lastWarning
class DwarfParser:
    ## Constructor for a DwarfParser object
    #
    # Parses the ELF file provided and maps symbols in the object
    #
    # @param[in] elf_path The path to the ELF file
    # @param[in] is_shared_lib If a shared library (DLL) is provided, put this flag to True
    def __init__(self, elf_path, is_shared_lib=False):
        
        self._die_typedef_list           = []
        self._die_variables_list         = []
        self._die_pointers_list          = []
        self._die_arrays_list            = []
        self._die_unions_list            = []
        self._die_basetypes_list         = []
        self._die_structuretypes_list    = []
        
        self._die_dict                  = {}
        
        with open(elf_path,'rb') as f:
            if not is_shared_lib:
                elffile = ELFFile(f)
                self._dwarfinfo = elffile.get_dwarf_info()
                self.is_little_endian = elffile.little_endian
            else:
                self.is_little_endian = True
                self._dwarfinfo = get_dwinfo_from_shared_lib(elf_path)
            
            # creates the {offset: DIE} dictionary
            self._populate_dicts()
            
            # populates the DIE variable list
            self._populate_lists()
                    
            for DIE in self._die_variables_list: 
                try:
                    # Base level symbol names, as those found in the list
                    base_name = DIE.attributes['DW_AT_name'].value.decode('utf-8')
                    value = DIE.attributes['DW_AT_location'].value
                    addr_format = DIE.attributes['DW_AT_location'].form
                    
                    # FIXME: Handmade address conversion (I don't know how to manage this with pyelftools...)
                    # only global symbols
                    try:
                        address = self._decode_address(value, addr_format, self.is_little_endian)
                        symbol = Symbol(address)
                        symbol.name = base_name
                        setattr(self, base_name, symbol)
                        
                        # now we need to populate the type, size and, if any, fields
                        # of the structure
                        self._populate_inner_levels(DIE, symbol)
                    except KeyError:
                        pass
                        
                except (KeyError, TypeError):
                    pass
    
    def iter_symbols(self):
        for sym_str in dir(self):
            sym = getattr(self, sym_str)
            if isinstance(sym, Symbol):
                yield sym
                
    def get_sym_by_name(self, sym_name):
        for sym in self.iter_symbols():
            if sym.name == sym_name:
                return sym
            elif sym.type == 'struct':
                for s in sym.unpack_sym():
                    if s.name == sym_name:
                        return s
        raise KeyError('No symbol with name: ' + str(sym_name) + ' found!')
    
    # ----- private ------ #
    
    ## Decode a symbol address
    #
    # Tries to decode the address of a symbol, given the address format
    #
    # @param[in] value The address value to decode
    # @param[in] addr_format The address format
    # @param[in] is_little_endian The endianness of the DWARF
    def _decode_address(self, value, addr_format, is_little_endian):
        if addr_format == 'DW_FORM_block1':
            if value[0] == 3:
                if is_little_endian:
                    address = (value[4] << 24) | (value[3] << 16) | (value[2] << 8) | value[1]
                else:
                    address = (value[1] << 24) | (value[2] << 16) | (value[3] << 8) | value[4]
                return address
            else:
                raise KeyError
        elif addr_format == 'DW_FORM_block':
            try:
                if value[0] == 3:
                    if is_little_endian:
                        address = (value[4] << 24) | (value[3] << 16) | (value[2] << 8) | value[1]
                    else:
                        address = (value[1] << 24) | (value[2] << 16) | (value[3] << 8) | value[4]
                    return address
            except IndexError:
                raise KeyError
            else:
                raise KeyError
        elif addr_format == 'DW_FORM_exprloc':
            if value[0] == 3:
                if is_little_endian:
                    address = (value[4] << 24) | (value[3] << 16) | (value[2] << 8) | value[1]
                else:
                    address = (value[1] << 24) | (value[2] << 16) | (value[3] << 8) | value[4]
                return address
            else:
                raise KeyError
        # NOT SUPPORTED YET, LOCATION_LIST!!
        elif addr_format == 'DW_FORM_data4':
            raise KeyError
        
        else:
            raise KeyError
        
    
    ## Populate a dictionary {offset: DIE}
    #
    # Scan the dwarfinfo in order to populate a dictionary variable
    # in the format {offset: DIE}. This dictionary will e used by the parser
    def _populate_dicts(self):
        for CU in self._dwarfinfo.iter_CUs():
            
            for DIE in CU.iter_DIEs():
                
                self._die_dict[str(DIE.offset)] = DIE
                
    
    ## Create a list with the variable DIEs
    #
    def _populate_lists(self):
        for CU in self._dwarfinfo.iter_CUs():
            
            for DIE in CU.iter_DIEs():
                
                if DIE.tag == 'DW_TAG_variable':
                    
                    self._die_variables_list.append(DIE)
                    
    ## Populate the inner level symbols
    #
    # This recursive method is called in order to retrieve type and size
    # info of the base symbols and to create the field struct's symbols
    # with all the relevant information.
    #
    # @param[in] DIE The Debugging Information Entity to scan for information
    # @param[in] symbol The base symbol to populate                           
    def _populate_inner_levels(self, DIE, symbol):
        
        die_type_offset = DIE.attributes['DW_AT_type'].value
        CU = DIE.cu
        
        # bug in pyelftools offset computation
        if (DIE.attributes['DW_AT_type'].form == 'DW_FORM_ref4' or
            DIE.attributes['DW_AT_type'].form == 'DW_FORM_ref_udata'):
            die_type_offset += CU.cu_offset
        
        inner_DIE = self._die_dict[str(die_type_offset)]
        CU = inner_DIE.cu
        
        # we want to search only for base types, recursively
        while   ((inner_DIE.tag != 'DW_TAG_base_type') and 
                (inner_DIE.tag != 'DW_TAG_pointer_type')):
            # handles structures
            if inner_DIE.tag == 'DW_TAG_structure_type':
                
                if inner_DIE.has_children:
                    symbol.type = 'struct'
                    try:
                        symbol.size = inner_DIE.attributes['DW_AT_byte_size'].value
                    except KeyError:
                        pass
                    for child in inner_DIE.iter_children():
                        if child.tag == 'DW_TAG_member':
                            field_name = child.attributes['DW_AT_name'].value.decode('utf-8')
                             
                            if type(child.attributes['DW_AT_data_member_location'].value) == int:
                                displ = child.attributes['DW_AT_data_member_location'].value
                            else:
                                displ = child.attributes['DW_AT_data_member_location'].value[1]
                             
                            address = symbol.address + displ
                            inner_symbol = Symbol(address)
                            #inner_symbol.name = field_name
                            inner_symbol.name = symbol.name + '.' + field_name
                            setattr(symbol, field_name, inner_symbol)
                            
                            
                            
                            try:
                                self._populate_inner_levels(child, inner_symbol)
                            except (KeyError, TypeError):
                                pass
                            
                            #------------- Bit Fields -------------#
                            try:
                                bit_size    = child.attributes['DW_AT_bit_size'].value
                                bit_offset  = child.attributes['DW_AT_bit_offset'].value
                                inner_symbol.type = 'bit_field'
                                inner_symbol.size = bit_size
                                setattr(inner_symbol, 'bit_offset', bit_offset)
                            except KeyError:
                                pass
                            
            
            # handles union
            if inner_DIE.tag == 'DW_TAG_union_type':
                if inner_DIE.has_children:
                    symbol.type = 'union'
                    for child in inner_DIE.iter_children():
                        if child.tag == 'DW_TAG_member':
                            field_name = child.attributes['DW_AT_name'].value.decode('utf-8')
                            
                            try:
                                if type(child.attributes['DW_AT_data_member_location'].value) == int:
                                    displ = child.attributes['DW_AT_data_member_location'].value
                                else:
                                    displ = child.attributes['DW_AT_data_member_location'].value[1]
                            except (KeyError, TypeError):
                                displ = 0
                                pass
                                     
                            address = symbol.address + displ
                            inner_symbol = Symbol(address)
                            #inner_symbol.name = field_name
                            inner_symbol.name = symbol.name + '.' + field_name
                            setattr(symbol, field_name, inner_symbol)
        
                            try:
                                self._populate_inner_levels(child, inner_symbol)
                                # for a union, the size of the members is the same
                                # as the size of the father
                                if inner_symbol.size != None:
                                    symbol.size = inner_symbol.size
                            except (KeyError, TypeError):
                                pass
                            
            # handles enumerations
            # for now, only size parsing is supported
            # TODO: Add support for children
            if inner_DIE.tag == 'DW_TAG_enumeration_type':
                base_type_size = inner_DIE.attributes['DW_AT_byte_size'].value
                symbol.size = base_type_size
                if symbol.size == 1:
                    symbol.type = 'unsigned char'
                elif symbol.size == 2:
                    symbol.type = 'unsigned short'
                elif symbol.size == 4:
                    symbol.type = 'unsigned int'
                else:
                    symbol.type = 'unsigned int'
                return
            
            # handles arrays
            if inner_DIE.tag == 'DW_TAG_array_type':
                if inner_DIE.has_children:
                    for child in inner_DIE.iter_children():
                        if child.tag == 'DW_TAG_subrange_type':
                            try:
                                array_dim = child.attributes['DW_AT_upper_bound'].value + 1
                                setattr(symbol, 'array_dim', array_dim)
                            except KeyError:
                                pass
                else:
                    try:
                        # this is needed in order to store total size of array types
                        symbol.size = inner_DIE.attributes['DW_AT_byte_size'].value
                    except KeyError:
                        pass
            try:
                # bug in pyelftools offset computation
                die_type_value = inner_DIE.attributes['DW_AT_type'].value
                if (inner_DIE.attributes['DW_AT_type'].form == 'DW_FORM_ref4'
                    or inner_DIE.attributes['DW_AT_type'].form == 'DW_FORM_ref_udata'):
                    die_type_value += CU.cu_offset
                inner_DIE = self._die_dict[str(die_type_value)]
            except (KeyError, TypeError):
                raise KeyError
            
        
        # at first handles the pointer type case in order to avoid infinite
        # recursive calls if the pointer points to the struct itself
        if inner_DIE.tag == 'DW_TAG_pointer_type':
            self._populate_pointer_type(inner_DIE, symbol)
            return 
                
        try:
            base_type_name = inner_DIE.attributes['DW_AT_name'].value.decode('utf-8')
            symbol.type = base_type_name
        except (KeyError, TypeError):
            pass
        
        # if it is not a array type, then store the size of the base type
        if symbol.size == None:
            base_type_size = inner_DIE.attributes['DW_AT_byte_size'].value
            symbol.size = base_type_size 
        return
    
    ## Populate the pointer type symbols
    #
    # This method is called when a pointer symbol is found. It is used to handle
    # the special case when a pointer base type is the same struct where the 
    # pointer is defined. Previously this case caused an infinite recursive call
    # with a stack overflow.
    #
    # @param[in] DIE The Debugging Information Entity to scan for information
    # @param[in] symbol The base symbol to populate   
    # @return DIE_copy A copy (modified by the method itself) of the passed 
    # input DIE
    def _populate_pointer_type(self, DIE, symbol):
        DIE_copy  = copy.copy(DIE)
        
        # the size is calculated from the size field of the pointer
        if symbol.size == None:
            base_type_size = DIE_copy.attributes['DW_AT_byte_size'].value
            symbol.size = base_type_size
        
        
        # what is the type of the pointer? we need to search for that
        
        die_type_offset = DIE_copy.attributes['DW_AT_type'].value
        CU = DIE_copy.cu
        
        # bug in pyelftools offset computation
        if (DIE_copy.attributes['DW_AT_type'].form == 'DW_FORM_ref4'
            or DIE_copy.attributes['DW_AT_type'].form == 'DW_FORM_ref_udata'):
            die_type_offset += CU.cu_offset
        
        # now we're going deeper
        DIE_copy = self._die_dict[str(die_type_offset)]
        #CU = inner_DIE.cu
        
        # have we reached the base types?
        while   ((DIE_copy.tag != 'DW_TAG_base_type') and 
                (DIE_copy.tag != 'DW_TAG_enumeration_type') and
                (DIE_copy.tag != 'DW_TAG_structure_type')):
            
            # no, let's go deeper
            DIE_copy = self._populate_pointer_type(DIE_copy, symbol)
        
        # yes, we populate the type field
        try:
            base_type_name = DIE_copy.attributes['DW_AT_name'].value.decode('utf-8')
            symbol.type = '* ' + base_type_name
            return DIE_copy
        except (KeyError, TypeError):
            return DIE_copy

## Member of the DwarfParser object
# 
# This class represents a symbol entity. The information it contains are: \n
#    
# - address: The symbol address in the MCU RAM;
# - type: The C type of the symbol
# - size: The size (in byte) of the symbol
# - value: The value of the symbol. In order to retrieve the value you should
# call external methods (such as those provided in master_commander) 
#
class Symbol(object):
    ## Symbol object constructor
    #
    # @param[in] address The MCU RAM address of the symbol
    def __init__(self, address):
        self.name = ''
        self.address = address
        self.type = None
        self.size = None
        self.value = None
        
    def __repr__(self):
        return str(self.name)
    
    ## Iterate children
    #
    # Iterate through the children of this Symbol.
    def iter_children(self):
        for child_str in dir(self):
            child = getattr(self, child_str)
            if isinstance(child, Symbol):
                yield child
    
    ## Unpack this Symbol
    #
    # Iterate through the children and go deeper until the 
    # last level has been reached.
    def unpack_sym(self):
        for child in self.iter_children():
            for c in child.iter_children():
                yield c
            yield child
            
    def unpack_sym_children(self):
        for child in self.iter_children():
            for c in child.iter_children():                
                yield c
    
    def unpack_sym_parent(self):
        for child in self.iter_children():
            for c in child.iter_children():                
                pass
            yield child
            
    def is_array_of_struct(self):
        if hasattr(self, 'array_dim') and self.type == 'struct':
            return True
        else:
            return False


class MacType(enum.IntEnum):
    DATATYPE_INVALID = 0
    DATATYPE_UINT8 = 1  
    DATATYPE_UINT16 = 2  
    DATATYPE_UINT32 = 3  
    DATATYPE_UINT64 = 4  
    DATATYPE_SINT8 = 5  
    DATATYPE_SINT16 = 6  
    DATATYPE_SINT32 = 7  
    DATATYPE_SINT64 = 8  
    DATATYPE_FLOAT32 = 9  
    DATATYPE_FLOAT64 = 10 
    DATATYPE_IS_ARRAY = 11 
    DATATYPE_IS_BITFIELD = 12 
    DATATYPE_IS_Q15_CELSIUS = 13
    
dwarf_to_mac_type = {'unsigned char'       : MacType.DATATYPE_UINT8.value,
                     'signed char'         : MacType.DATATYPE_SINT8.value,
                     'char'                : MacType.DATATYPE_SINT8.value,
                     'unsigned short'      : MacType.DATATYPE_UINT16.value,
                     'short unsigned int'  : MacType.DATATYPE_UINT16.value,
                     'signed short'        : MacType.DATATYPE_SINT16.value,
                     'short'               : MacType.DATATYPE_SINT16.value,
                     'short int'           : MacType.DATATYPE_SINT16.value,
                     'unsigned int'        : MacType.DATATYPE_UINT32.value,
                     'signed int'          : MacType.DATATYPE_SINT32.value,
                     'int'                 : MacType.DATATYPE_SINT32.value,
                     'unsigned long'       : MacType.DATATYPE_UINT64.value,
                     'long unsigned int'   : MacType.DATATYPE_UINT64.value,
                     'signed long'         : MacType.DATATYPE_SINT64.value,
                     'long'                : MacType.DATATYPE_SINT64.value,
                     'long int'            : MacType.DATATYPE_SINT64.value,
                     'unsigned long long'  : MacType.DATATYPE_UINT64.value,
                     'signed long long'    : MacType.DATATYPE_SINT64.value,
                     'long long'           : MacType.DATATYPE_SINT64.value,
                     'long long int'       : MacType.DATATYPE_SINT64.value,
                     'float'               : MacType.DATATYPE_FLOAT32.value,
                     'double'              : MacType.DATATYPE_FLOAT64.value,
                     'bit_field'           : MacType.DATATYPE_IS_BITFIELD.value} 
  
def main(elf_path, addr_format, xml_path, device='MCU_NUCLEUS', version=False, shared_lib=False):
    def populate_element(sym, displacement=0, struct_array_dim=0):
        el = ET.SubElement(dev, 'Variable')
        # name
        el.set('name', sym.name)
        # address
        addr_el = ET.SubElement(el, 'Address')
        if addr_format == 0:
            addr_el.text = str(sym.address)
        elif addr_format == 1:
            addr_el.text = hex(sym.address)
        else:
            logger.error('Invalid address format. Can be 0 or 1. %s given', addr_format)
            sys.exit(-1)
            
        # dim
        if struct_array_dim != 0:
            dim = struct_array_dim
        else:
            if hasattr(sym, 'array_dim'):
                dim = sym.array_dim
            else:
                dim = 1
        dim_el = ET.SubElement(el, 'Dim')
        dim_el.text = str(dim)
        # type
        type_el = ET.SubElement(el, 'Type')
        try:
            type_el.text = str(dwarf_to_mac_type[sym.type])
        except KeyError:
            type_el.text = str(MacType.DATATYPE_INVALID.value)
            logger.warning('Type %s undefined for symbol %s', sym.type, sym.name)
        # size
        size_el = ET.SubElement(el, 'Size')
        size_el.text = str(sym.size)
        # displacement
        displ_el = ET.SubElement(el, 'Displacement')
        displ_el.text = str(displacement)
        
        # bit field type specific
        if sym.type == 'bit_field':
            # bit offset
            bit_offset_el = ET.SubElement(el, 'BitOffset')
            bit_offset_el.text = str(sym.bit_offset)
    
    if version:
        print('Dwarf Parser CLI version: ' + __version__)
    
    # process the ELF file
    try:
        dwarf_hdlr = DwarfParser(elf_path, shared_lib)
    except BaseException as e:
        sys.exit(-1)
        
    # generate the output file
    # header
    root = ET.Element('DwarfParser')
    dev = ET.SubElement(root, 'Device')
    dev.set('name', device)
    # symbols
    for s in dwarf_hdlr.iter_symbols():
        displacement = 0
        struct_array_dim = 0
        if s.is_array_of_struct():
            logger.debug('Found array of struct: %s', s.name)
            logger.debug('Displacement: %s', s.size)
            logger.debug('Array dim: %s', s.array_dim)
            displacement = s.size
            struct_array_dim = s.array_dim
        if s.type != 'struct' and s.type != 'union':
            populate_element(s, displacement)
        for c in s.unpack_sym():
            if c.type != 'struct' and c.type != 'union':
                populate_element(c, displacement, struct_array_dim)
    
    # write to file
    tree = ET.ElementTree(root)
    try:
        tree.write(xml_path)
    except BaseException as e:
        sys.exit(-1)
    
        
# CLI for MAC integration
if __name__ == '__main__':
    # arguments definition and parsing
    parser = argparse.ArgumentParser(description='Dwarf Parser ' + __version__)
    parser.add_argument('elf_path', type=str, help = 'The path to the ELF File')
    parser.add_argument('addr_format', type=int, help = 'The address format: 0=decimal 1=hexadecimal')
    parser.add_argument('xml_path', type=str, help = 'The path to the generated XML file')
    parser.add_argument('--device', type=str, default='MCU_NUCLEUS', help = 'Device used internally in MAC')
    parser.add_argument('--version', '-v', help = 'Show version info', action='store_true')
    parser.add_argument('--shared_lib', '-so', help = 'Shall parse a DLL', action='store_true')
    args = parser.parse_args()
    main(args.elf_path, args.addr_format, args.xml_path, args.device, args.version, args.shared_lib)
    sys.exit(0)