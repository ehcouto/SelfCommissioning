##
# @file
# @brief   Main PC interface for the Master&Commander serial protocol.
#
# @details  
# 
# @author  Leonardo Ricupero
#
#
# @copyright Copyright 2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL


##--------------------------Import Files---------------------------##

import logging
import os
import struct
import sys

import enum
import serial

sys.path.extend(['.', '..'])
import dwarf_parser.dwarf_parser as dwarf_parser

logging.basicConfig()
logger = logging.getLogger(__name__)
logger.setLevel(logging.ERROR)

# -------- ARCH CONSTANTS --------- #
LITTLE_ENDIAN   = '<'
BIG_ENDIAN      = '>'

LONG_MEMORY     = 'I'
SHORT_MEMORY    = 'H'

ALIGNMENT_BYTE  = 0
ALIGNMENT_WORD  = 1

##--------------------------Classes definitions -------------------##

## Architecture Enumeration
#
# This enumeration class defines the architectures currently supported
class EnumArch(enum.IntEnum):
    FREESCALE_DSC   = 0
    RENESAS_RX62T   = 1
    FREESCALE_KV3X  = 2
    
## Architecture class definition
#
# Defines the properties of a MCU architecture
class Arch:
    ## Class constructor
    #
    # Creates a Arch object which is used by the MasterCommander object in order
    # to properly build and parse packets to / from the MCU.
    #
    # @param[in] ARCH The EnumArch constant which defines the architecture to be
    # used
    def __init__(self, ARCH):
        if ARCH == EnumArch.RENESAS_RX62T:
            self.address_fmt        = LITTLE_ENDIAN + SHORT_MEMORY
            self.address_alignment  = ALIGNMENT_BYTE
            self.data_fmt            = LITTLE_ENDIAN
        elif ARCH == EnumArch.FREESCALE_DSC:
            self.address_fmt        = LITTLE_ENDIAN + SHORT_MEMORY
            self.address_alignment  = ALIGNMENT_BYTE
            self.data_fmt            = LITTLE_ENDIAN
        elif ARCH == EnumArch.FREESCALE_KV3X:
            self.address_fmt        = LITTLE_ENDIAN + LONG_MEMORY
            self.address_alignment  = ALIGNMENT_BYTE
            self.data_fmt            = LITTLE_ENDIAN
        else:
            raise RuntimeError('Invalid architecture selected: ' + str(ARCH))
        self.arch = ARCH
        
    ##--------------------------Public Methods----------------------##
    
    ## Get a Python struct format
    #
    # Given a symbol type from the dwarf_parser Symbol object this method returns
    # the correct Python struct format.
    #
    # @param[in] s_type The C type field taken from a dwarf_parser.Symbol object  
    # @return The Python struct compatible format associated to the s_type input
    # C format
    def get_format(self, s_type):
        if self.arch == EnumArch.RENESAS_RX62T:
            
            fmt = self.data_fmt
            
            if s_type == 'unsigned char':
                fmt += 'B'
            elif s_type == 'signed char':
                fmt += 'b'
            elif s_type == 'unsigned short':
                fmt += 'H'
            elif s_type == 'short':
                fmt += 'h'
            elif s_type == 'signed short':
                fmt += 'h'
            elif s_type == 'signed short int':
                fmt += 'h'
            elif s_type == 'unsigned int':
                fmt += 'I'
            elif s_type == 'int':
                fmt += 'i'
            elif s_type == 'unsigned long':
                fmt += 'L'
            elif s_type == 'signed long':
                fmt += 'l'
            elif s_type == 'long':
                fmt += 'l'
            elif s_type == 'unsigned long long':
                fmt += 'Q'
            elif s_type == 'long long':
                fmt += 'q'
            elif s_type == 'float':
                fmt += 'f'
            elif s_type == 'double':
                fmt += 'd'
            else:
                raise MasterCommanderException('Unknown format detected: ' + s_type)
            return fmt
        
        elif self.arch == EnumArch.FREESCALE_KV3X:
            
            fmt = self.data_fmt
            
            if s_type == 'unsigned char':
                fmt += 'B'
            elif s_type == 'signed char':
                fmt += 'b'
            elif s_type == 'char':
                fmt += 'b'
            elif s_type == 'unsigned short':
                fmt += 'H'
            elif s_type == 'signed short':
                fmt += 'h'
            elif s_type == 'short':
                fmt += 'h'
            elif s_type == 'unsigned int':
                fmt += 'I'
            elif s_type == 'int':
                fmt += 'i'
            elif s_type == 'unsigned long':
                fmt += 'L'
            elif s_type == 'signed long':
                fmt += 'l'
            elif s_type == 'long':
                fmt += 'l'
            elif s_type == 'float':
                fmt += 'f'
            elif s_type == 'double':
                fmt += 'f'
            else:
                raise MasterCommanderException('Unknown format detected: ' + s_type)
            return fmt
        
        elif self.arch == EnumArch.FREESCALE_DSC:
            
            fmt = self.data_fmt
            
            if s_type == 'unsigned char':
                fmt += 'B'
            elif s_type == 'signed char':
                fmt += 'b'
            elif s_type == 'char':
                fmt += 'b'
            elif s_type == 'unsigned short':
                fmt += 'H'
            elif s_type == 'signed short':
                fmt += 'H'
            elif s_type == 'short':
                fmt += 'h'
            elif s_type == 'unsigned int':
                fmt += 'H'
            elif s_type == 'int':
                fmt += 'h'
            elif s_type == 'unsigned long':
                fmt += 'L'
            elif s_type == 'signed long':
                fmt += 'l'
            elif s_type == 'long':
                fmt += 'l'
            elif s_type == 'float':
                fmt += 'f'
            elif s_type == 'double':
                fmt += 'f'
            else:
                raise MasterCommanderException('Unknown format detected: ' + s_type)
            return fmt


## Master&Commander protocol Fixed position Bytes Enumeration
#
class EnumPktFix(enum.IntEnum):
    SOB = 0
    STS = 1
    CMD = 1
    LEN = 2

## Master&Commander protocol constants Bytes Enumeration
#
class EnumPktBytes(enum.IntEnum):
    SOB                  = 0x2B
    # commands
    CMD_GETINFO          = 0xc0
    CMD_READMEM          = 0x01
    CMD_WRITEMEM         = 0x02
    CMD_READMEM_EX       = 0x04
    CMD_WRITEMEM_EX      = 0x05
    CMD_READLF           = 0x23
    CMD_WRITELF          = 0x24
    CMD_WRITELF_BYTE     = 0x25
    CMD_SINGLEREQ        = 0x27
    CMD_SETUPREC         = 0x09
    CMD_STARTREC         = 0xc1
    CMD_STOPREC          = 0xc2
    CMD_GETRECSTS        = 0xc3
    CMD_GETRECBUFF       = 0xc4
    # confirmation codes
    STS_OK               = 0x00
    # error codes
    STC_INVCMD           = 0x81
    STC_CMDCSERR         = 0x82
    STC_CMDTOOLONG       = 0x83
    STC_RSPBUFFOVF       = 0x84
    STC_INVBUFF          = 0x85
    STC_INVSIZE          = 0x86
    STC_SERVBUSY         = 0x87
    STC_NOTINIT          = 0x88
    STC_EACCESS          = 0x89
    STC_PIPEERR          = 0x8C
    STC_FLOATDISABLED    = 0x90

## Master&Commander Exception class
#
class MasterCommanderException(Exception):
    pass

## Master&Commander Parser Exception class
#
class ParserException(Exception):
    pass

## Master&Commander Configuration class
#
class MCComConfig(object):
    _fields_ = (
        ('prot_ver', '<B'),
        ('cfg_flags','<B'),
        ('cfg_bus_width','<B'),
        ('glob_ver_major', '<B'),
        ('glob_ver_minor', '<B'),
        ('comm_buffer_size', '<B'),
        ('rec_buffer_size', '<H'),
        ('rec_timebase', '<H')
    )
    
    def __init__(self):
        for name, fmt in type(self)._fields_:
            setattr(self, name, None)

    def _unpack(self, data, offset = 0):
        for name, fmt in type(self)._fields_:
            value = struct.unpack_from(fmt, data, offset)
            logger.info('%s: %s', name, str(value))
            offset += struct.calcsize(fmt)
            setattr(self, name, value[0])
            


    
## Master&Commander Main Class
#
# This class provides a API for the Master&Commander communication
# protocol.
class MasterCommander:
    ## Class constructor
    #
    # @param[in] serial_port The COM port used for the serial communication
    # @param[in] serial_baud The serial baudrate selected for the communication
    # This must match the baudrate of the board
    # @param[in] serial_timeout Time to wait before the reading method returns
    # if there are no characters on the buffer
    # @param[in] arch The architecture constant as defined in EnumArch
    # @param[in] mc_c_buffer_size The Master&Commander uC-side buffer size
    def __init__(self, serial_port, serial_baud, serial_timeout=0.2, arch = EnumArch.RENESAS_RX62T, mc_c_buffer_size = 35):
        try:
            self.sercom = serial.Serial(None, serial_baud, timeout = serial_timeout)
            self.sercom.port = serial_port
            if self.sercom.isOpen() == False:
                self.sercom.open()
                logger.debug('COM port opened')
        except serial.SerialException as e:
            raise MasterCommanderException('Error opening COM port: ' + str(e))
        self.arch = Arch(arch)
        self.com_config = MCComConfig()
        self.mc_c_buffer_size = mc_c_buffer_size
        self._address_size = struct.calcsize(self.arch.address_fmt)
        # ask for the configuration
        self.read_board_config()
        
    ##--------------------------Public Methods----------------------##
    
    ## Read the board configuration
    #
    # Read the MasterCommander related board configuration. This will 
    # update the user defined mc_c_buffer_size reading it directly on the
    # board FW.
    #
    # @remark Call this method before using advanced features such the Recorder. 
    def read_board_config(self):
        try:
            self._ask_for_config(self.com_config)
            self.mc_c_buffer_size = self.com_config.comm_buffer_size
        except BaseException as e:
            raise MasterCommanderException('Error getting board configuration: ' + str(e))
    
    ## Sends a SINGLEREQ packet
    #
    # This method sends a serial packet requesting fixed floating point
    # values to write and returning a fixed number of floating point 
    # variables.
    #
    # @remark The number of input and output values must match with
    # the embedded code configuration
    #
    # @param[in] values List of floating point values to write.
    # @param[in] n_exp_values Number of expected values to read
    # @return List of read floating point values
    def send_singlereq_pkt(self, index, values, n_exp_values):
        FMT = 'f'
        checksum    = 0
        sum         = 0
        
        # value to write is 4 bytes long
        msg_len = len(values) * struct.calcsize(FMT) + 1
        
        # builds the packet
        pkt = bytearray()
        pkt.append(EnumPktBytes.SOB)
        pkt.append(EnumPktBytes.CMD_SINGLEREQ)
        pkt.append(msg_len)
        pkt.append(index)
        # pack addresses and values
        data_fmt    = self.arch.data_fmt + FMT
        
        for val in values:
            pkt += struct.pack(data_fmt, val)
        
        # checksum computation
        for c in pkt[1:]:
            sum += (c)
        checksum = tohex(~(sum) + 1)
        pkt.append(checksum)

        logger.debug('Message to be sent: %s', pkt.hex())
        elab_pkt = self._build_repl_sob(pkt)
        # sends the packet
        self.sercom.write(elab_pkt)
        # now parse the response
        expected_len = n_exp_values * struct.calcsize(FMT) + 3
        try:
            answer = self._parser(expected_len)
        except ParserException:
            logger.error('Parser Error...')
            raise
        
        offset = 0
        ret_list = []
        for i in range(n_exp_values):
            value = struct.unpack_from(data_fmt, answer, offset)[0]
            ret_list.append(value)
            offset += struct.calcsize(FMT)
        
        return ret_list
    
    ## Writes an arbitrary amount of symbols from MCU RAM
    # (optimized for non contiguous variables)
    #    
    # This method performs a certain amount of write operations,
    # based on the input symbol list. \n
    # It performs the WRITELF and WRITELF_BYTE requests, which use a common 
    # buffer to group together the variables, thus optimizing the number and 
    # size of packets.
    #
    # @param[in] symbols_tuple Tuple of symbols objects to write
    def write_multi_sparse(self, symbols_tuple):
        # unpack structs symbols from struct types
        # we need a list because it's not immutable
        symbols_list = []
        self._unpack_struct_symbols(symbols_tuple, symbols_list)
        # now convert back to tuple
        symbols = tuple(sym for sym in symbols_list)
        
        # sort the symbols with respect to the address
        sorted_symbols = sorted(symbols, key = lambda sym: sym.address)
        
        # evaluate the number and type of packets needed
        byte_sym_list   = []
        word_sym_list   = []
        size_byte_pkt      = 0
        size_word_pkt      = 0
        n_writelf_pkt      = 0
        n_writelf_byte_pkt = 0
        for sym in sorted_symbols:
            # is the symbol size 1 byte?
            if sym.size == 1:
                # add the symbol to the 1-byte symbols list
                byte_sym_list.append(sym) 
                size_byte_pkt += sym.size + self._address_size
            else:
                # add the symbol to the other symbols' list
                word_sym_list.append(sym)
                size_word_pkt   += sym.size * self._address_size
        
        # number of packets computation
        if not byte_sym_list:
            n_writelf_byte_pkt = 0
        else:
            n_writelf_byte_pkt = (size_byte_pkt // self.mc_c_buffer_size) + 1
        if not word_sym_list:
            n_writelf_pkt = 0
        else:
            n_writelf_pkt = (size_word_pkt // self.mc_c_buffer_size) + 1
        
        logger.info('Number of WRITELF packets needed: %d', n_writelf_pkt)
        logger.info('Number of WRITELF_BYTE packets needed: %d', n_writelf_byte_pkt)
        
        
        ################# WRITELF Requests processing ##########################
        
        # No WRITELF request needed
        if n_writelf_pkt == 0:
            pass
        
        # Multiple WRITELF requests needed
        elif n_writelf_pkt > 1:
            address_value_couples = ()
            size_to_write = size_word_pkt
            n_pkt_to_send = n_writelf_pkt
            
            while (n_pkt_to_send):
                
                if n_pkt_to_send > 1:
                    
                    this_pkt_size = 0
                    
                    # if we have more than one packet to send, we fill the buffer
                    while this_pkt_size <= self.mc_c_buffer_size:
                        # retrieve last symbol from the list until the buffer isn't full
                        sym = word_sym_list.pop()
                        this_pkt_size += sym.size * self._address_size 
                        # updates the size to write
                        size_to_write -= sym.size * self._address_size
                        
                        # if this symbol fills the buffer, we backup it and exit
                        # the loop
                        if (this_pkt_size > self.mc_c_buffer_size):
                            word_sym_list.append(sym)
                            break
                        
                        # build the request argument
                        offset = 0
                        n_addr_per_sym = sym.size // 2
                        if n_addr_per_sym >= 2:
                            logger.debug('Symbol %s covers %d addresses', str(sym), n_addr_per_sym)
                            val_fmt = self.arch.get_format(sym.type)
                            val = struct.pack(val_fmt, sym.value)
                            
                            for i in range(n_addr_per_sym):
                                data_fmt = self.arch.data_fmt + 'H'
                                val_chunk = struct.unpack_from(data_fmt, val, offset)
                                address_value_couples += ((sym.address + 2*i, val_chunk[0]),)
                                offset += struct.calcsize(data_fmt)
                        else:
                            
                            val_fmt = self.arch.get_format(sym.type)
                            val = struct.pack(val_fmt, sym.value)
                            data_fmt = self.arch.data_fmt + 'H'
                            val_chunk = struct.unpack_from(data_fmt, val, offset)
                            address_value_couples += ((sym.address, val_chunk[0]),)
                        
                        logger.debug('current packet size: %d', this_pkt_size)
                        logger.debug('Remaining size to write: %d', size_to_write)
                    
                    # perform the request
                    logger.info('Address / values couples are: %s', str(address_value_couples))
                    try:
                        self.send_writelf_req(*address_value_couples)
                    except BaseException as e:
                        raise MasterCommanderException('Request error: ' + str(e))
                    address_value_couples = ()
                    n_pkt_to_send -= 1
                    
                # last packet to send
                else:
                    # last packet to send. we already have to pop the backupped 
                    # symbol
                    address_value_couples = ()
                    this_pkt_size = 0
                    while(len(word_sym_list)):
                        sym = word_sym_list.pop()
                        this_pkt_size += sym.size * self._address_size 
                        # updates the size to write
                        size_to_write -= sym.size * self._address_size
                        
                        # there shouldn't be more packets, but if the symbol size
                        # is large there is the possibility to overflow. In this
                        # case we need an extra packet
                        if (this_pkt_size > self.mc_c_buffer_size):
                            logger.warning('Spare symbol found. Adding an extra packet!')
                            word_sym_list.append(sym)
                            n_pkt_to_send += 1
                            break
                        
                        n_addr_per_sym = sym.size // 2
                        if n_addr_per_sym >= 2:
                            logger.debug('Symbol %s covers %d addresses', str(sym), n_addr_per_sym)
                            val_fmt = self.arch.get_format(sym.type)
                            val = struct.pack(val_fmt, sym.value)
                            offset = 0
                            for i in range(n_addr_per_sym):
                                data_fmt = self.arch.data_fmt + 'H'
                                val_chunk = struct.unpack_from(data_fmt, val, offset)
                                address_value_couples += ((sym.address + 2*i, val_chunk[0]),)
                                offset += struct.calcsize(data_fmt)
                        else:
                            offset = 0
                            val_fmt = self.arch.get_format(sym.type)
                            val = struct.pack(val_fmt, sym.value)
                            data_fmt = self.arch.data_fmt + 'H'
                            val_chunk = struct.unpack_from(data_fmt, val, offset)
                            address_value_couples += ((sym.address, val_chunk[0]),)
                        
                    # perform the request
                    logger.info('Address / values couples are: %s', str(address_value_couples))
                    try:
                        self.send_writelf_req(*address_value_couples)
                    except BaseException as e:
                        raise MasterCommanderException('Request error:  ' + str(e))
                    n_pkt_to_send -= 1
                
                
        # One WRITELF request needed
        else:
            address_value_couples = ()
            for sym in word_sym_list:
                # build the request argument
                n_addr_per_sym = sym.size // 2
    
                if n_addr_per_sym >= 2:
                    logger.debug('Symbol %s covers %d addresses', str(sym), n_addr_per_sym)
                    val_fmt = self.arch.get_format(sym.type)
                    val = struct.pack(val_fmt, sym.value)
                    offset = 0
                    for i in range(n_addr_per_sym):
                        data_fmt = self.arch.data_fmt + 'H'
                        val_chunk = struct.unpack_from(data_fmt, val, offset)
                        address_value_couples += ((sym.address + 2*i, val_chunk[0]),)
                        offset += struct.calcsize(data_fmt)
                else:
                    offset = 0
                    val_fmt = self.arch.get_format(sym.type)
                    val = struct.pack(val_fmt, sym.value)
                    data_fmt = self.arch.data_fmt + 'H'
                    val_chunk = struct.unpack_from(data_fmt, val, offset)
                    address_value_couples += ((sym.address, val_chunk[0]),)
            # perform the request
            logger.info('Address / values couples are: %s', str(address_value_couples))
            try:
                self.send_writelf_req(*address_value_couples)
            except BaseException as e:
                raise MasterCommanderException('Request error:  ' + str(e))
            
        ################# WRITELF_BYTE Requests processing #####################
        
        # No WRITELF_BYTE request needed
        if n_writelf_byte_pkt == 0:
            pass
        
        # Multiple WRITELF_BYTE requests needed
        elif n_writelf_byte_pkt > 1:
            
            address_value_couples = ()
            size_to_write = size_byte_pkt
            n_pkt_to_send = n_writelf_byte_pkt
            
            while (n_pkt_to_send):
                
                if n_pkt_to_send > 1:
                    
                    this_pkt_size = 0
                    
                    # if we have more than one packet to send, we fill the buffer
                    while this_pkt_size <= self.mc_c_buffer_size:
                        # retrieve last symbol from the list until the buffer isn't full
                        sym = byte_sym_list.pop()
                        this_pkt_size += 1 + self._address_size
                        # updates the size to write
                        size_to_write -= 1 + self._address_size
                        
                        if (this_pkt_size > self.mc_c_buffer_size):
                            byte_sym_list.append(sym)
                            break
                        
                        # build the request argument
                        val_fmt = self.arch.get_format(sym.type)
                        val = ord(struct.pack(val_fmt, sym.value))
                        address_value_couples += ((sym.address, val),)
                        
                        logger.debug('current packet size: %d', this_pkt_size)
                        logger.debug('Remaining size to write: %d', size_to_write)
                    
                    # perform the request
                    logger.info('Address / values couples are: %s', str(address_value_couples))
                    try:
                        self.send_writelf_byte_req(*address_value_couples)
                    except BaseException as e:
                        raise MasterCommanderException('Request error:  ' + str(e))
                    n_pkt_to_send -= 1
                    
                # last packet to send
                else:
                    # last packet to send. we already popped out the symbol
                    # build the request argument
                    address_value_couples = ()
                    this_pkt_size = 0
                    while(len(byte_sym_list)):
                        sym = byte_sym_list.pop()
                        this_pkt_size += 1 + self._address_size
                        # updates the size to write
                        size_to_write -= 1 + self._address_size
                        
                        if (this_pkt_size > self.mc_c_buffer_size):
                            logger.warning('Spare symbol found. Adding an extra packet!')
                            byte_sym_list.append(sym)
                            n_pkt_to_send += 1
                            break
                        
                        # build the request argument
                        val_fmt = self.arch.get_format(sym.type)
                        val = ord(struct.pack(val_fmt, sym.value))
                        address_value_couples += ((sym.address, val),)
                        
                    # perform the request
                    logger.info('Address / values couples are: %s', str(address_value_couples))
                    try:
                        self.send_writelf_byte_req(*address_value_couples)
                    except BaseException as e:
                        raise MasterCommanderException('Request error:  ' + str(e))
                    n_pkt_to_send -= 1

        # One WRITELF_BYTE request needed
        else:
            address_value_couples = ()
            for sym in byte_sym_list:
                
                # build the request argument
                val_fmt = self.arch.get_format(sym.type)
                val = ord(struct.pack(val_fmt, sym.value))
                address_value_couples += ((sym.address, val),)
            # perform the request
            logger.info('Address / values couples are: %s', str(address_value_couples))
            try:
                self.send_writelf_byte_req(*address_value_couples)
            except BaseException as e:
                raise MasterCommanderException('Request error:  ' + str(e))
    
    ## Read an arbitrary amount of symbols from MCU RAM 
    # (optimized for non contiguous variables)
    #    
    # This method performs a certain amount of read operations,
    # based on the input symbol list. \n
    # It performs the READLF request, which uses a common buffer to group
    # together the variables, thus optimizing the number and size of packets
    #
    # @todo This routine needs proper refactoring. Pack common code in a subroutine.
    # 
    # @param[in] symbols_tuple Tuple of symbols objects to read
    def read_multi_sparse(self, symbols_tuple):
        # unpack structs symbols from struct types
        symbols_list = []
        self._unpack_struct_symbols(symbols_tuple, symbols_list)
        symbols = tuple(sym for sym in symbols_list)
        
        # sort the symbols with respect to the address
        # inverse order because we will pop out the symbols (stack-like)
        sorted_symbols = sorted(symbols, key = lambda sym: sym.address, reverse=True)
        # save the symbols list for population
        sorted_sym_bkp = sorted(symbols, key = lambda sym: sym.address)
        
        # evaluate the number of packets needed
        total_pkt_size = 0
        for sym in sorted_symbols:
            if sym.size == 1:
                #total_pkt_size += 2
                total_pkt_size += self._address_size
            else:
                total_pkt_size += sym.size // 2 * self._address_size
        
        total_pkt_size += 3 # accounts for CMD, LEN, CHKSUM bytes
        n_pkt = (total_pkt_size // self.mc_c_buffer_size) + 1
        logger.debug('Calculated total packet size: %d', total_pkt_size)
        logger.info('Number of read packet requested: %d', n_pkt)
        
        
        ###################### Requests processing #############################
        raw_data = bytearray()
        if n_pkt > 1:
            addresses = ()
            bytes_to_read = total_pkt_size
            n_pkt_to_send = n_pkt
            
            while (n_pkt_to_send):
                
                if n_pkt_to_send > 1:
                    
                    this_pkt_size = 0
                    
                    # if we have more than one packet to send, we fill the buffer
                    while (this_pkt_size <= self.mc_c_buffer_size - 3):
                        # retrieve last symbol from the list until the buffer is full
                        sym = sorted_symbols.pop()
                        # updates the size to write
                        if sym.size == 1:
                            this_pkt_size += self._address_size
                            bytes_to_read -= self._address_size
                        else:
                            this_pkt_size += sym.size // 2 * self._address_size
                            bytes_to_read -= sym.size // 2 * self._address_size
                        
                        if (this_pkt_size > self.mc_c_buffer_size - 3):
                            sorted_symbols.append(sym)
                            break
                        
                        # For each address in the packet we request one single word
                        # we need to handle large symbols giving more addresses
                        n_addr_per_sym = (sym.size // 2)
                        if n_addr_per_sym >= 2:
                            logger.debug('Symbol %s covers %d addresses', str(sym), n_addr_per_sym)
                            for i in range(n_addr_per_sym):
                                addresses += (sym.address + 2*i,)
                        else:
                            logger.debug('Symbol %s covers 1 address')
                            addresses += (sym.address,)
                        
                        logger.debug('current packet size: %d', this_pkt_size)
                        logger.debug('Remaining size to read: %d', bytes_to_read)
                    
                    logger.debug('Addresses to request are: %s', str(addresses))
                    try:
                        raw_data += self.send_readlf_req(*addresses)
                        addresses = ()
                    except BaseException as e:
                        raise MasterCommanderException('Request error:  ' + str(e))
                    n_pkt_to_send -= 1
                    
                # last packet to send
                else:
                    # last packet to send. we already popped out the symbol
                    # build the request argument
                    addresses = ()
                    this_pkt_size = 0
                    while(len(sorted_symbols)):
                        sym = sorted_symbols.pop()
                        # updates the size to write
                        if sym.size == 1:
                            this_pkt_size += self._address_size
                            bytes_to_read -= self._address_size
                        else:
                            this_pkt_size += sym.size // 2 * self._address_size
                            bytes_to_read -= sym.size // 2 * self._address_size
                        
                        if (this_pkt_size > self.mc_c_buffer_size):
                            logger.warning('Spare symbol found. Adding an extra packet!')
                            sorted_symbols.append(sym)
                            n_pkt_to_send += 1
                            break
                        
                        # build the request argument
                        # For each address in the packet we request one single word
                        # we need to handle large symbols giving more addresses
                        n_addr_per_sym = (sym.size // 2)
                        if n_addr_per_sym >= 2:
                            logger.debug('Symbol %s covers %d addresses', str(sym), n_addr_per_sym)
                            for i in range(n_addr_per_sym):
                                addresses += (sym.address + 2*i,)
                        else:
                            logger.debug('Symbol %s covers 1 address', str(sym))
                            addresses += (sym.address,)
                        
                    # perform the request
                    logger.debug('Addresses to request are: %s', str(addresses))
                    try:
                        raw_data += self.send_readlf_req(*addresses)
                    except BaseException as e:
                        raise MasterCommanderException('Request error:  ' + str(e))
                    n_pkt_to_send -= 1
        
        # process the single packet request    
        else:
            addresses = ()
            for sym in sorted_sym_bkp:
                # For each address in the packet we request one single word
                # we need to handle large symbols giving more addresses
                n_addr_per_sym = (sym.size // 2)
                if n_addr_per_sym >= 2:
                    logger.debug('Symbol %s covers %d addresses', str(sym), n_addr_per_sym)
                    for i in range(n_addr_per_sym):
                        addresses += (sym.address + 2*i,)
                else:
                    logger.debug('Symbol %s covers 1 address', str(sym))
                    addresses += (sym.address,)
            logger.debug('Addresses to request are: %s', str(addresses))    
            try:
                raw_data += self.send_readlf_req(*addresses)
            except BaseException as e:
                raise MasterCommanderException('Request error: ' + str(e))
        
        ########################## Data processing #############################
        logger.info('Read operations done! Now populate symbol values...')
        offset = 0
        for sym in sorted_sym_bkp:
            # how many words?
            n_words = (sym.size // 2)
            # if zero this is a byte
            if n_words == 0:
                logger.debug('Unpacking a single byte symbol: %s', str(sym))
                value = struct.unpack_from(self.arch.get_format(sym.type), raw_data, offset)
                offset += 2
            else:
                try:
                    value = struct.unpack_from(self.arch.get_format(sym.type), raw_data, offset)
                except BaseException as e:
                    raise MasterCommanderException('Symbol value unpacking went WRONG! Details: ' + str(e))
                offset += 2 * n_words
            
            logger.info('Populating symbol %s with value %d', str(sym), value[0])
            sym.value = value[0] 
        
        
    ## Read an arbitrary amount of symbols from MCU RAM
    #    
    # This method performs a certain amount of read operations,
    # based on the input symbol list. \n
    # It tries to optimize the number of read packets by grouping in the same
    # packet the variables which are located in the same memory areas
    #
    # @param[in] symbols_tuple Tuple of symbols objects to read
    #
    # @todo needs to be optimized for packet number.
    def read_multiple(self, symbols_tuple):
        # unpack structs symbols from struct types
        symbols_list = []
        self._unpack_struct_symbols(symbols_tuple, symbols_list)
        symbols = tuple(sym for sym in symbols_list)
        
        # first we evaluate the total memory displacement
        # retrieve symbols
        address_list    = []
        # displacement
        for sym in symbols:
            address_list.append(sym.address)
            
        start_address = min(address_list)
        end_address = max(address_list)
        last_sym_index = address_list.index(end_address)
        end_pointer = end_address + symbols[last_sym_index].size
        start_pointer = start_address
        
        total_displacement = end_pointer - start_address
        
        # evaluate the number of packets needed
        n_pkt = int(total_displacement // self.mc_c_buffer_size) + 1
        logger.info('Number of read packet requested: %d', n_pkt)
        mem_list = []
        # do we need more than one packet?
        if n_pkt > 1:
            logger.info('Maximum buffer size exceeded. we need more than one packet')
            
            # process the requests
            while (end_pointer - start_pointer) > 0:    
                if (end_pointer - start_pointer) < self.mc_c_buffer_size:
                    size = end_pointer - start_pointer
                else:
                    size = self.mc_c_buffer_size
                logger.debug('Evaluated memory size: %d', size)
                
                # if there is at least one symbol, then send the request, else skip
                f_to_send = 0
                for sym_address in address_list:
                    if sym_address < end_pointer and sym_address > start_pointer:
                        f_to_send = 1
                
                # if there's no symbol in this memory slice, don't send the packet
                if not f_to_send:
                    continue
                
                try:
                    raw_data = self.send_readram_req(start_pointer, size)
                    address = start_pointer
                    
                    for c in raw_data:
                        mem_list.append((address,c))
                        address += 1
                    logger.debug('Memory structure: %s', str(mem_list))
                    
                except MasterCommanderException:
                    raise MasterCommanderException('Error in reading request')
                start_pointer += size
        
        # just one request needed
        else:
            logger.info('We need just one packet')
            
            size = end_pointer - start_address
            try:
                raw_data = self.send_readram_req(start_address, size)
                address = start_address
                #mem_list = []
                # @todo make it more pythonic
                for c in raw_data:
                    mem_list.append((address,c))
                    address += 1
                logger.debug('Memory structure: %s', str(mem_list))
            except MasterCommanderException:
                raise MasterCommanderException('Error in reading request')
            
        # raw memory data process
        logger.info('Read operations done. Now processing data')
        #mem_list_copy = mem_list[:]
        for sym in symbols:
            # retrieve the python struct format from symbol type
            fmt = self.arch.get_format(sym.type)
            for mem_item in mem_list:
                if sym.address == mem_item[0]:
                    # address found. now unpack data
                    start_index = mem_list.index(mem_item)
                    end_index = start_index + sym.size
                    sub_list = mem_list[start_index:end_index]
                    data = [y[1] for y in sub_list]
                    data = bytes(data)
                    logger.debug('data to unpack: %s', data)
                    value = struct.unpack(fmt, data)
                    sym.value = value[0]
                    break
    
    
    ## Writes an arbitrary amount of symbols from MCU RAM
    #    
    # This method performs a certain amount of write operations,
    # based on the input symbol list. \n
    # It tries to optimize the number of write packets by grouping in the same
    # packet the variables which are located in contiguous memory areas
    #
    # @param[in] symbols_tuple Tuple of symbols objects to write
    def write_multiple(self, symbols_tuple):
        # unpack structs symbols from struct types
        # we need a list because it's not immutable
        # TOTAL_BUFFER_SIZE - CMD - LEN - SIZE - ADDRESS
        max_values_size = self.mc_c_buffer_size - 5 
        symbols_list = []
        self._unpack_struct_symbols(symbols_tuple, symbols_list)
        # now convert back to tuple
        symbols = tuple(sym for sym in symbols_list)
        
        # we need to sort the symbols with respect to the address
        write_list = []
        sorted_symbols = sorted(symbols, key = lambda sym: sym.address)
        
        # now scan for contiguous variables
        cont_list = [sorted_symbols[0]]
        
        logger.info('Scanning for contiguous symbols')
        for i in range(len(sorted_symbols)):
            
            logger.debug('symbol address: %s', sorted_symbols[i])
            mem_span = sorted_symbols[i].address + sorted_symbols[i].size 
            
            
            try:
                next_sym_address = sorted_symbols[i + 1].address
            except IndexError:
                logger.debug('End of list reached')
                write_list.append(cont_list)
                break
            
            # contiguous symbols case. if the size of contiguous area is greater
            # than the buffer, we choose to split the area
            cont_mem_span = sorted_symbols[i + 1].address + sorted_symbols[i + 1].size - cont_list[0].address
            logger.debug('Contiguous memory span: %d', cont_mem_span)
            if ((mem_span == next_sym_address) and (cont_mem_span < max_values_size)):
                logger.debug('Contiguous symbol')
                cont_list.append(sorted_symbols[i + 1])
            
            # non contiguous symbol found
            else:
                logger.debug('Non contiguous symbol')
                write_list.append(cont_list)
                # reinitialize the contiguous symbols list
                cont_list = [sorted_symbols[i + 1]]
        
        # now proceed with the writing
        logger.info('**** Proceeding with write operations ****')
        logger.info('Packets required: %d', len(write_list))
        for sym_list in write_list:
            # first we handle the simple case: just one var to write
            if len(sym_list) == 1:
                logger.info('Single write var packet going...')
                self.write_var(sym_list[0])
                #return
            # now the case in which we have contiguous variables
            else:
                logger.info('Multiple write var packet going...')
                values = ()
                address = sym_list[0].address
                for field in sym_list:
                    # retrieve the python struct format from symbol type
                    fmt = self.arch.get_format(field.type)
                    if field.value == None:
                        logger.warning('Value field of symbol at address %s was None. Replacing with 0...', field)
                        value = 0
                    else:
                        value = field.value
                    
                    values += ((value, fmt),)
                
                try:    
                    self.send_writeram_req(address, *values)
                except (MasterCommanderException, ParserException):
                    raise
    
    ## Read a variable from MCU RAM
    # 
    # This method searches for the address and size of the provided
    # symbol and assembles the packet to send with the correct format to
    # the MCU. The read values are written to the Symbol object values fields. \n
    # If a struct Symbol is provided the method tries to assemble a single packet
    # and then populates the corresponding Symbol objects values fields.
    #
    # @param[in] symbol The Symbol object to read from the MCU RAM.
    # @return The base Symbol value field. 
    def read_var(self, symbol):
        # counts and records the number of struct fields (if any)
        n_fields = 0
        fields_list = []
        for str_attribute in dir(symbol):
            attribute = getattr(symbol, str_attribute)
            if isinstance(attribute, dwarf_parser.Symbol):
                logger.debug('Found symbol attribute: %s', str_attribute)
                fields_list.append(attribute)
                n_fields += 1
                
        # we need to sort the fields with respect to the address
        # @todo should try to solve this by overriding __dir__ in  Symbol class
        fields_list = sorted(fields_list, key = lambda sym: sym.address)
        
        # retrieve symbol information
        try:
            address = symbol.address
            size = symbol.size
        except NameError:
            raise MasterCommanderException('Error accessing symbol address and / or size!')
        
        if (size >= self.mc_c_buffer_size):
            raise MasterCommanderException('Symbol size ' + str(size) +' exceeds MasterCommander buffer size!')
        
        # sends the request
        try:
            data = self.send_readram_req(address, size)
        except MasterCommanderException:
            raise
        
        # populates the symbol value(s) according to the format
        if n_fields:
            offset = 0
            
            for field in fields_list:
                # retrieve the python struct format from symbol type
                fmt = self.arch.get_format(field.type)
                value = struct.unpack_from(fmt, data, offset)
                # populates the field value
                field.value = value[0]
                
                offset += struct.calcsize(fmt)
                   
        else:
            # retrieve the python struct format from symbol type
            fmt = self.arch.get_format(symbol.type)
            
            value = struct.unpack(fmt, data)
            symbol.value = value[0]
                
            return symbol.value
    
    
    ## Write a variable to MCU RAM
    # 
    # This method searches for the address and size of the provided
    # symbol and assembles the packet to send with the correct format to
    # the MCU. \n
    # The values to be written are taken from the Symbol object values fields or
    # from the values tuple (if provided). Priority is given to the tuple. \n
    # If a struct Symbol is provided the method tries to assemble a single packet.
    #
    # @param[in] symbol The Symbol object to write in the MCU RAM.
    # @param[in] *values A tuple composed by tuples of (value, format) couples.
    # The format is the one used by the Python struct module.
    # @todo check if the buffer size is less than symbol size. If so, raise
    # an exception
    def write_var(self, symbol, *values):
        
        # counts and records the number of struct fields (if any)
        n_fields = 0
        fields_list = []
        for str_attribute in dir(symbol):
            attribute = getattr(symbol, str_attribute)
            if isinstance(attribute, dwarf_parser.Symbol):
                logger.debug('Found symbol attribute: %s', str_attribute)
                fields_list.append(attribute)
                n_fields += 1
                
        # we need to sort the fields with respect to the address
        # @todo should try to solve this by overriding __dir__ in  Symbol class
        fields_list = sorted(fields_list, key = lambda sym: sym.address)    
             
        # retrieve symbol information
        try:
            address = symbol.address
            size = symbol.size
        except NameError:
            raise MasterCommanderException('Error accessing symbol address and / or size!')
        
        # TOTAL_BUFFER_SIZE - CMD - LEN - SIZE - ADDRESS
        max_values_size = self.mc_c_buffer_size - 5 
        if (size >= max_values_size):
            raise MasterCommanderException('Symbol size ' + str(size) +' exceeds MasterCommander buffer size!')
        
        # Retrieve values information. 
        # Priority is given to the values tuple if provided
        if len(values):
            # size coherence
            # size computation is yielded by the fmt
            size_values = 0
            for v in values:
                size_values += struct.calcsize(v[1])
            logger.debug('Calculated size to write is %d', size)
            # we proceed even if the size is not coherent. Perhaps we should raise
            # an exception?
            if size != size_values:
                logger.warning('Symbol size and the size of the values provided are different!!')
                logger.warning('Write request goes on...')
            try:    
                self.send_writeram_req(address, *values)
            except MasterCommanderException:
                raise
        
        # if values are not provided we take them from symbol.value    
        else:
            # is the symbol a struct?
            if n_fields:
                offset = 0
            
                for field in fields_list:
                    # retrieve the python struct format from symbol type
                    fmt = self.arch.get_format(field.type)
                    if field.value == None:
                        value = 0
                        logger.warning('Value field of symbol at address %s was None. Replacing with 0...', field)
                    else:
                        value = field.value
                    
                    values += ((value, fmt),)
                
                try:    
                    self.send_writeram_req(address, *values)
                except MasterCommanderException:
                    raise
                   
            # or is it a simple variable?
            else:
                # retrieve the python struct format from symbol type
                fmt = self.arch.get_format(symbol.type)
                if symbol.value == None:
                    value = 0
                    logger.warning('Value field of symbol at address %s was None. Replacing with 0...', symbol)
                else:
                    value = symbol.value
                if value != None:
                    values += (value, fmt)
                        
                    try:    
                        self.send_writeram_req(address, values)
                    except MasterCommanderException:
                        raise 
                else:
                    raise MasterCommanderException('No values to write found!!')
    
    ## Read an array
    #
    # Read an array, putting the values in the value field of the provided 
    # Symbol object.
    #
    # @param[in] sym The array Symbol object
    # @param[in] n_items Number of items of the array to read. If left to 0 will read
    # all the array elements
    # @param[in] offset Index of the item form where to start to read. If 0 will start
    # to read from the first item
    def read_array(self, sym, n_items=0, offset=0):
        # initial checks
        # make sure we are reading an array
        assert hasattr(sym, 'array_dim') == True
        if n_items > sym.array_dim:
            n_items = sym.array_dim
            logger.warning('Specified number of items to read is too high!')
            logger.warning('Array dimension is: %d', sym.array_dim)
        elif n_items == 0:
            n_items = sym.array_dim
        if offset > sym.array_dim:
            offset = 0
            logger.warning('Specified offset is too high!')
            logger.warning('Array dimension is: %d', sym.array_dim)
        if n_items + offset > sym.array_dim:
            logger.warning('Number of items plus offset is out of boundary!')
            logger.warning('Will put offset = 0!')
            offset = 0
        
        # start address computation
        start_address = sym.address + sym.size * offset
        # size computation
        size = sym.size * n_items
        # send the read request
        raw_data = self.send_readram_req(start_address, size)
        # populate the symbol values
        sym.value = []
        offset = 0
        for i in range(n_items):
            value = struct.unpack_from(self.arch.get_format(sym.type), raw_data, offset)[0]
            sym.value.append(value)
            offset += sym.size
            
    ## Write an array
    #
    # Write array values, taking the values from the value field of the provided 
    # Symbol object.
    #
    # @param[in] sym The array Symbol object
    # @param[in] n_items Number of items of the array to write. If left to 0 will write
    # all the array elements
    # @param[in] offset Index of the item form where to start to write. If 0 will start
    # to write from the first item
    def write_array(self, sym, n_items=0, offset=0):
        # initial checks
        # make sure we are reading an array
        assert hasattr(sym, 'array_dim') == True
        if n_items > sym.array_dim:
            n_items = sym.array_dim
            logger.warning('Specified number of items to write is too high!')
            logger.warning('Array dimension is: %d', sym.array_dim)
        elif n_items == 0:
            n_items = sym.array_dim
        if offset > sym.array_dim:
            offset = 0
            logger.warning('Specified offset is too high!')
            logger.warning('Array dimension is: %d', sym.array_dim)
        if n_items + offset > sym.array_dim:
            logger.warning('Number of items plus offset is out of boundary!')
            logger.warning('Will put offset = 0!')
            offset = 0
        # checks on the Symbol value list
        if n_items > len(sym.value):
            logger.warning('Not enough values to write!')
            logger.warning('Will truncate to the available values')
            n_items = len(sym.value)
        if offset > len(sym.value):
            logger.warning('Offset is too high for the available values!')
            logger.warning('Will put offset = 0')
            offset = 0
        if n_items + offset > sym.array_dim:
            logger.warning('Number of items plus offset is out of boundary!')
            logger.warning('Will put offset = 0!')
            offset = 0
        
        
        # start address computation
        start_address = sym.address + sym.size * offset
        # values tuple construction
        values = ()
        fmt = self.arch.get_format(sym.type)
        for i in range(n_items):
            v = sym.value[i + offset]
            values += ((v, fmt),)
        
        # send the write request
        self.send_writeram_req(start_address, *values)
        
    
    ##--------------------------Low level API----------------------##
    
    ## Sends a WRITELF_BYTE RAM request
    #
    # This low level API method sends a serial packet requesting a data RAM 
    # region to write with the WRITELF_BYTE command (optimized for sparse writing)
    #
    # @remark This method is suited for 8 bit variables. If variables with size greater or 
    # equal to 16 bit need to be written, use the send_writelf_req.
    #
    # @param[in] *address_value_couples Tuple of symbol (address, value) tuples that specifies
    # the address to write and the value (8 bit sized).
    # @return The parser answer (should be empty)
    def send_writelf_byte_req(self, *address_value_couples):
        checksum    = 0
        sum         = 0
            
        # for each value to write, we have 2 bytes of address, and 1 byte of value
        msg_len = len(address_value_couples) * (1 + self._address_size)
         
        # builds the packet
        pkt = bytearray()
        pkt.append(EnumPktBytes.SOB)
        pkt.append(EnumPktBytes.CMD_WRITELF_BYTE)
        pkt.append(msg_len)
        
        # pack addresses and values
        address_fmt = self.arch.address_fmt
        data_fmt    = self.arch.data_fmt + 'B'
        
        for addr, val in address_value_couples:
            pkt += struct.pack(address_fmt, addr)
            pkt += struct.pack(data_fmt, val)
        
        # checksum computation
        for c in pkt[1:]:
            sum += (c)
        checksum = tohex(~(sum) + 1)
        pkt.append(checksum)

        logger.debug('Message to be sent: %s', pkt.hex())
        elab_pkt = self._build_repl_sob(pkt)
        # sends the packet
        self.sercom.write(elab_pkt)
        # now parse the response
        expected_len = 3
        try:
            answer = self._parser(expected_len)
        except ParserException:
            raise
        
        return answer
    
    ## Sends a WRITELF RAM request
    #
    # This low level API method sends a serial packet requesting a data RAM 
    # region to write with the WRITELF command (optimized for sparse writing)
    #
    # @remark This method can be used only for variables with size greater or equal to
    # 16 bit. For 8 bit variables the send_writelf_byte_req must be used.
    #
    # @param[in] *address_value_couples Tuple of symbol (address, value) tuples that specifies
    # the address to write and the value.
    # @return The parser answer (should be empty)
    def send_writelf_req(self, *address_value_couples):
        checksum    = 0
        sum         = 0
        
        # for each value to write, we have N bytes of address, and 2 bytes of value              
        msg_len = len(address_value_couples) * (2 + self._address_size) 
         
        # builds the packet
        pkt = bytearray()
        pkt.append(EnumPktBytes.SOB)
        pkt.append(EnumPktBytes.CMD_WRITELF)
        pkt.append(msg_len)
        
        # pack addresses and values
        address_fmt = self.arch.address_fmt
        data_fmt    = self.arch.data_fmt + 'H'
        
        for addr, val in address_value_couples:
            pkt += struct.pack(address_fmt, addr)
            pkt += struct.pack(data_fmt, val)
        
        # checksum computation
        for c in pkt[1:]:
            sum += (c)
        checksum = tohex(~(sum) + 1)
        pkt.append(checksum)

        logger.debug('Message to be sent: %s', pkt)
        elab_pkt = self._build_repl_sob(pkt)
        # sends the packet
        self.sercom.write(elab_pkt)
        # now parse the response
        expected_len = 3
        try:
            answer = self._parser(expected_len)
        except ParserException:
            raise
        
        return answer
            
    
    ## Sends a READLF RAM request
    #
    # This low level API method sends a serial packet requesting a data RAM 
    # region to read with the READLF command (optimized for sparse reading)
    #
    # @param[in] *addresses Tuple of symbol addresses to read
    # @return The raw data read from memory
    def send_readlf_req(self, *addresses):
        checksum = 0
        sum = 0
        
        msg_len = len(addresses) * self._address_size
        
        # builds the packet
        pkt = bytearray()
        pkt.append(EnumPktBytes.SOB)
        pkt.append(EnumPktBytes.CMD_READLF)
        pkt.append(msg_len)
        # address width is variable depending on the architecture
        for add in addresses:
            pkt += struct.pack(self.arch.address_fmt, add)
        
        # checksum computation
        for c in pkt[1:]:
            sum += c
        checksum = tohex(~(sum) + 1)
        pkt.append(checksum)

        logger.debug('Message to be sent: %s', pkt)
        elab_pkt = self._build_repl_sob(pkt)
        # sends the packet
        self.sercom.write(elab_pkt)
        # now parse the response
        # len of pkt is : SIZE + SOB + STS + CHECKSUM
        expected_len = len(addresses) * 2 + 3
        try:
            data = self._parser(expected_len)
        except ParserException:
            raise
        
        return data
        
    ## Sends a read RAM request
    #
    # This low level API method sends a serial packet requesting a data RAM 
    # region to read.
    # 
    # @param[in] start_address The RAM address (in bytes) from which start reading
    # @param[in] size Amount of data (bytes) to read, starting at start_address
    # @return The raw data taken from the response packet
    def send_readram_req(self, start_address, size):
        checksum = 0
        sum = 0
        
        # builds the packet
        pkt = bytearray()
        pkt.append(EnumPktBytes.SOB)
        pkt.append(EnumPktBytes.CMD_READMEM_EX)
        msg_len = self._address_size + 1 # start_address + size
        pkt.append(msg_len)
        pkt.append(size)
        # address width is 2 bytes, fixed
        address_fmt = self.arch.address_fmt
        pkt += struct.pack(address_fmt, start_address)
        
        # checksum computation
        for c in pkt[1:]:
            sum += (c)
        checksum = tohex(~(sum) + 1)
        pkt.append(checksum)

        logger.debug('Message to be sent: %s', pkt.hex())
        elab_pkt = self._build_repl_sob(pkt)
        # sends the packet
        self.sercom.write(elab_pkt)
        # now parse the response
        # len of pkt is : SIZE + SOB + STS + CHECKSUM
        expected_len = size + 3
        try:
            data = self._parser(expected_len)
        except ParserException as e:
            raise MasterCommanderException('Parser Error: ' + str(e))
        
        return data


    ## Sends a write RAM request
    #
    # This low level API method sends a serial packet requesting a data RAM 
    # region to write.
    #
    # @param[in] start_address The RAM address (in bytes) from which start writing
    # @param[in] *values The arbitrary number of tuples in the format (value, fmt)
    # @return The raw data taken from the answer packet. Normally this should
    # be a void list.
    def send_writeram_req(self, start_address, *values):
        checksum = 0
        sum = 0
        # size computation is yielded by the fmt
        size = 0
        for v in values:
            size += struct.calcsize(v[1])
        
        logger.debug('Calculated size to write is %d', size)
        
        msg_len = 1 + self._address_size + size # size + start_address + data
        # builds the packet
        pkt = bytearray()
        pkt.append(EnumPktBytes.SOB)
        pkt.append(EnumPktBytes.CMD_WRITEMEM_EX)
        pkt.append(msg_len)
        pkt.append(size)
        # address width is 2 bytes, fixed
        address_fmt = self.arch.address_fmt
        pkt += struct.pack(address_fmt, start_address)
        
        for v in values:
            # v[1] is the format
            # v[0] is the value
            pkt += struct.pack(v[1], v[0])
        
        # checksum computation
        for c in pkt[1:]:
            sum += (c)
        checksum = tohex(~(sum) + 1)
        pkt.append(checksum)
        
        logger.debug('Message to be sent: %s', pkt.hex())
        elab_pkt = self._build_repl_sob(pkt)
        # sends the packet
        self.sercom.write(elab_pkt)
        # now wait for the response
        try:
            answer = self._parser(3)
        except ParserException as e:
            raise MasterCommanderException('Parser Error: ' + str(e))
        
        return answer
    
    ##--------------------------Private Methods----------------------##
    
    ## Ask for the Board configuration
    #
    # Send a packet with the board info request and populate the
    # MCComConfig object accordingly 
    #
    # @param[in] cfg_handler The MCComConfig to be populated
    def _ask_for_config(self, cfg_handler):
        logger.info('Asking for board configuration')
        try:
            data = self._send_board_info_req()
        except BaseException as e:
            raise MasterCommanderException('Error getting the board configuration data: ' + str(e))
        
        # populate the handler
        try:
            cfg_handler._unpack(data)
        except BaseException as e:
            raise MasterCommanderException('Error unpacking config data: ' + str(e))
    
    ## Send a board info request packet
    #
    def _send_board_info_req(self):
        checksum    = 0
        sum         = 0
        
        # builds the packet
        pkt = bytearray()
        pkt.append(EnumPktBytes.SOB)
        pkt.append(EnumPktBytes.CMD_GETINFO)

        # checksum computation
        for c in pkt[1:]:
            sum += (c)
        checksum = tohex(~(sum) + 1)
        pkt.append(checksum)

        logger.debug('Message to be sent: %s', (pkt))
        elab_pkt = self._build_repl_sob(pkt)
        # sends the packet
        self.sercom.write(elab_pkt)
        # now parse the response
        expected_len = 38
        try:
            answer = self._parser(expected_len)
        except ParserException:
            raise
    
        return answer
    
    

    # ----- private ------ #
    
    ## Response packet Parser
    #
    # @param[in] size_to_parse Expected size of the packet to be parsed
    # @param[in] max_garbage_size The maximum number of invalid characters to
    # be read before raising a ParserException
    # @return The packet data content, not including the checksum, SOB 
    # and STS bytes
    def _parser(self, size_to_parse, max_garbage_size=2):
        garbage_size = 0
        f_last_sob = 0
        n_to_parse = 0
        rx_checksum = 0
        pkt = bytearray()
        
        # payload does not include SOB, STS, CHKSUM
        payload = size_to_parse - 3
        size_to_parse -= 1 
        # is this a simple response packet?
        if payload == 0:
            n_words = 0
        else:
            n_words = payload // 2 
        
        
        n = self.sercom.inWaiting()
        logger.debug('In RX buffer: %d', n)
        
        # main parser outer loop
        while garbage_size < max_garbage_size:
            
            # main parser inner loop          
            while 1:
                c = self.sercom.read(1)
                # handles loss of communication
                if c == b'':
                    garbage_size += 1
                    logger.warning('Empty character read!. Garbage size: %d', garbage_size)
                    break
                 
                # SOB handling
                if c == bytes([EnumPktBytes.SOB]):
                    # handle replicated SOB
                    f_last_sob ^= 1
                    if f_last_sob:
                        logger.debug('SOB received and discarded')
                        # discard the character
                        break
                    logger.debug('Replicated SOB received. Continue...')
                 
                # Status byte handling
                if f_last_sob:
                    logger.debug('Status Byte handling: %s', (c))
                    if c != bytes([EnumPktBytes.STS_OK]):
                        logger.warning('Status Code Not OK: %s', EnumPktBytes(c))
                        raise ParserException('Status Code Not OK: ' + 
                                              str(EnumPktBytes(c)))
                    # reset the receiving process
                    # @todo checksum computation formally wrong
                    #pkt = c
                    #rx_checksum += ord(c)
                    n_to_parse = size_to_parse
                    f_last_sob = 0
                 
                # parsing data    
                if n_to_parse != 0:
                    pkt += c
                    rx_checksum += ord(c)
                    n_to_parse -= 1
                    # was this the last character to parse?
                    if n_to_parse == 0:
                        # yes, go with the checks
                        logger.debug('Raw packet: %s', pkt)
                        # checksum check
                        rx_checksum = tohex(rx_checksum, 8)
                        if (rx_checksum & 0xFF) != 0:
                            logger.warning('Wrong checksum: %s', hex(rx_checksum))
                            raise ParserException('Checksum error')
                        
                        # status code check
                        if (pkt[0]) != EnumPktBytes.STS_OK:
                            logger.warning('Status Code Not OK: %s', hex(pkt[EnumPktFix.STS]))
                            raise ParserException('Status Code Not OK: ' + 
                                                  str(EnumPktBytes((pkt[0]))))
                        
                        answer = pkt[1 :-1] 
                        logger.debug('Returned pkt is: %s', answer)
                        return answer
        # did we receive an error code?
        if len(pkt) == 2:
            # calculate the checksum
            rx_checksum = tohex(~(pkt[0]) + 1)
            # checksum wrong
            if rx_checksum != (pkt[1]):
                raise ParserException('Wrong response length and wrong checksum!')
            else:
                raise ParserException('Parser exited with error code: ' + 
                                      str(EnumPktBytes((pkt[0]))))
            
        raise ParserException('Too much empty characters received!')   
    
    ## Builds a packet with replicated SOBs
    #
    # @param[in] pkt The original packet, with non-replicated SOBs
    # @return The packet with replicated SOBs
    def _build_repl_sob(self, pkt):
        # split the pkt with the SOBs
        split_pkt = pkt.split(bytes([EnumPktBytes.SOB]))
        
        # first element is always the SOB
        res_pkt = bytearray()
        res_pkt.append(EnumPktBytes.SOB)
        # now build the new packet and replicate the intermediate SOBs
        for slice in split_pkt[1:-1]:
            res_pkt += slice
            res_pkt.append(EnumPktBytes.SOB)
            res_pkt.append(EnumPktBytes.SOB)
        
        # handles last element of original pkt. Is it a SOB?
        if split_pkt[-1] == EnumPktBytes.SOB:
            # yes, replicate
            res_pkt.append(EnumPktBytes.SOB)
            res_pkt.append(EnumPktBytes.SOB)
        else:
            # no, just add it
            res_pkt += split_pkt[-1]
        logger.debug('Packet after replicated SOB: %s', (res_pkt))
        return res_pkt
    
    def _unpack_struct_symbols(self, symbols, unpacked_symbols):
        for sym in symbols:
            logger.debug('unpacking symbol at address: %s', hex(sym.address))
            is_sym_parent = 0
            # we look for symbol's children
            for str_attribute in dir(sym):
                attribute = getattr(sym, str_attribute)
                if isinstance(attribute, dwarf_parser.Symbol):
                    is_sym_parent = 1
                    logger.debug('Found child: %s', str_attribute)
                    self._unpack_struct_symbols((attribute,), unpacked_symbols)
            
            # add this to the unpacked symbols, if it's a base symbol
            if not is_sym_parent:
                unpacked_symbols.append(sym)
        
        
## Helper function which converts any value (signed or unsigned) into raw hex
#
# @param[in] val The value (signed or unsigned) to convert
# @param[in] nbits The width, in bits, of the value to convert
# @return The raw hex value of the input value
def tohex(val, nbits = 8):
    return ((val + (1 << nbits)) % (1 << nbits))


if __name__ == '__main__':
    #--------- DEFINE HERE THE ARCHITECTURE OF THE TARGET ---------#
    arch = EnumArch.FREESCALE_KV3X
     
    comport = 34
    baudrate = 57600
    sym = dwarf_parser.DwarfParser('./Windy_Strip_Bpm.out')
    
    mc = MasterCommander(comport, baudrate, 0.2, arch) 
    
    values= [0xDE,0xAD,0xBE,0xEF]
    sym.ClassB_Signature.value = values
    
    mc.write_array(sym.ClassB_Signature)
    
    mc.read_array(sym.ClassB_Signature)
    
    for v in sym.ClassB_Signature.value:
        print(hex(v))
    
