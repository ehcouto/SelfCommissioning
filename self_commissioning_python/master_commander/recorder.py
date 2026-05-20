## @file recorder.py
#
# Recorder feature for the Master&Commander serial protocol.
#
# @author Leonardo Ricupero

import time
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import sys
sys.path.extend(['.', '..'])
import dwarf_parser.dwarf_parser as dwarf_parser
from master_commander import *

logging.basicConfig()
logger = logging.getLogger(__name__)
logger.setLevel(logging.WARNING)

## Master&Commander Recorder
#
# This class implements a Recorder, a Master&Commander feature
# that allows to sample fast changing variables directly into a 
# MCU RAM buffer, and transfer the data when the recording is complete.\n
# It is possible to use a trigger variable (with trigger slope and threshold)
# to trigger the acquisition starting.
class Recorder(object):
    ## Class constructor
    #
    # @param[in] name The user defined name of this Recorder
    # @param[in] com_hdlr The Master&Commander object that will be used to send and
    # receive data from the target MCU
    # @param[in] trigger_mode Set the trigger mode (0 = disabled, 1 = _/, 2 = \_ )
    # @param[in] n_samples Number of samples to acquire
    # @param[in] n_post_tr_samples Number of post-trigger samples to take
    # @param[in] time_div Choose the sub-sample factor
    # @param[in] trig_symbol The Symbol used as the trigger variable
    # @param[in] tr_comp_mode The threshold comparison mode (0 = unsigned, 1 = signed)
    # @param[in] tr_thr_value The trigger threshold value
    # @param[inout] symbols The list of Symbols to acquire. The acquired data will be
    # stored in a new Symbol attribute: rec_buff
    def __init__(self, 
                 name,
                 com_hdlr,
                 trigger_mode,
                 n_samples,
                 n_post_tr_samples,
                 time_div,
                 trig_symbol,
                 tr_comp_mode,
                 tr_thr_val,
                 symbols):
        
        # TODO: arguments validation
        self.name = name
        if not isinstance(com_hdlr, MasterCommander):
            raise MasterCommanderException('Error creating Recorder: must provide a valid MasterCommander COM handler!!')
        self.com_hdlr = com_hdlr
        self.trigger_mode = trigger_mode
        self.n_samples = n_samples
        self.n_post_tr_samples = n_post_tr_samples
        self.time_div = time_div
        if isinstance(trig_symbol, dwarf_parser.DwarfParser):
            self.tr_var_addr = trig_symbol.address
            self.tr_var_size = trig_symbol.size
        else:
            self.tr_var_addr = 0
            self.tr_var_size = 1
        self.tr_comp_mode = tr_comp_mode
        self.tr_thr_val = tr_thr_val
        self.symbols = symbols
        self.n_rec_vars = len(self.symbols)
        # recorder buffer size requires board info
        self.buffer_size = self.com_hdlr.com_config.rec_buffer_size
        if not self.buffer_size:
            raise MasterCommanderException('Invalid recorder buffer size provided: get the board configuration first!')
        
        timebase = self.com_hdlr.com_config.rec_timebase
        # decode the timebase
        # zzyy yyyy yyyy yyyy
        # z are the measure unit
        # zz = 0x0 -> sec
        # zz = 0x4 -> ms
        # zz = 0x8 -> us
        # zz = 0xC -> ns
        zz = (timebase & 0xC000) >> 12
        if zz == 0x0:
            unit = 10 ** 0
        elif zz == 0x4:
            unit = 10 ** -3
        elif zz == 0x8:
            unit = 10 ** -6
        elif zz == 0xC:
            unit = 10 ** -9
        else:
            raise MasterCommanderException('Error decoding recorder timebase!')
        val = timebase & 0x3FFF
        self.timebase = val * unit
        logger.info('Recorder timebase: %f', self.timebase)
        
        # calculate available and requested size
        sym_size = 0
        for sym in symbols:
            sym_size += sym.size
        self.requested_size = sym_size * self.n_samples
        self.sample_size = sym_size
        logger.info('Requested size for recorder %s: %d', self.name, self.requested_size)
        logger.info('Sample size is: %d', self.sample_size)
        
        if self.requested_size > self.buffer_size:
            raise MasterCommanderException('Not enough buffer space for this recorder! Requested size: ' + 
                                           str(self.requested_size) + ' , available: ' + str(self.buffer_size))
            
        # put the rec_buff attribute in the registered symbols
        for s in symbols:
            setattr(s, 'rec_buff', [])
            
    ## Recorder setup
    #
    # Call this method to send a setup command to the MCU.
    #
    # @remark This must be called right after the Recorder construction and
    # before any other operation.
    def setup(self):
        try:
            self._send_setup_cmd()
        except BaseException as e:
            raise MasterCommanderException('Error during recorder setup: ' + str(e))
    
    ## Recorder start
    #
    # Sends a start command to the MCU. The embedded Recorder will look for the 
    # trigger condition to happen and will trigger the internal acquisition if 
    # needed.
    def start(self):
        try:
            self._send_start_cmd()
        except BaseException as e:
            raise MasterCommanderException('Error starting the recorder: ' + str(e))
        
    
    ## Recorder stop
    #
    # Force a stop event to happen. The trigger condition will be forced if needed, the
    # data will be acquired on the internal MCU buffer.
    def stop(self):
        try:
            self._send_stop_cmd()
        except BaseException as e:
            raise MasterCommanderException('Error stopping the recorder: ' + str(e))
    
    ## Get the acquired data
    #
    # This method tries to populate the requested Symbols with the recorded
    # data.\n
    # The stop method will be called if the recorder is still running.
    #
    # @remark The start method has to be called first
    #
    # @todo Handle exception if already stopped. Right now, the stop method is called
    # in any case, and a exception is thown if the acquisition is already stopped
    #
    # @param[in] timeout The maximum time to wait (blocking) for the buffer info answer. If 0 the
    # method will not block and immediately return with the requested data or an exception if
    # the data is not ready
    #
    def get_data(self, timeout):
        # stop the recorder first
        # TODO: handle exception if already stopped
        try:
            self._send_stop_cmd()
        except BaseException as e:
            raise MasterCommanderException('Error stopping the recorder: ' + str(e))
        
        # handle the buffer info request
        info = None
        t0 = time.time()
        while timeout >= 0:
            try:
                info = self._send_getbuff_cmd()
                break
            except ParserException:
                timeout -= (time.time() - t0)
                pass
        
        if not info:
            raise MasterCommanderException('Failed to get recorder buffer info')
        
        # info ok
        buff_start, start_idx = struct.unpack('<HH', info)
        logger.debug('Rec buff start address: %s', hex(buff_start))
        logger.debug('Rec sample index: %d', start_idx)
        start_idx *= self.sample_size
        logger.debug('Rec buffer index: %d', start_idx)
        
        # now fetch data from the circular buffer
        # buffer limit calculation
        buff_end = buff_start + self.requested_size
        data_to_request = self.requested_size
        com_buffer_size = self.com_hdlr.mc_c_buffer_size
        logger.debug('Rec buff end address: %d', buff_end)
        raw_data = ''
        
        while data_to_request:
            buff_ptr = buff_start + (start_idx % self.requested_size)
            logger.debug('Pointing to: %d', buff_ptr)
            # are we limited by com buffer size?
            if data_to_request > com_buffer_size:
                # limited by com buffer size
                # circular buffer handling
                # did we reach the end?
                if (buff_ptr + com_buffer_size) > buff_end:
                    # yes, read till the end of circular buffer
                    raw_data += self.com_hdlr.send_readram_req(buff_ptr, (buff_end - buff_ptr))
                    data_to_request -= (buff_end - buff_ptr)
                    start_idx = 0
                else:
                    # no, read the maximum data size
                    raw_data += self.com_hdlr.send_readram_req(buff_ptr, com_buffer_size)
                    data_to_request -= com_buffer_size
                    start_idx += com_buffer_size
            else:
                # not limited: read the whole data
                # did we reach the end?
                if (buff_ptr + data_to_request) > buff_end:
                    # yes, read till the end of circular buffer
                    raw_data += self.com_hdlr.send_readram_req(buff_ptr, (buff_end - buff_ptr))
                    data_to_request -= (buff_end - buff_ptr)
                    start_idx = 0
                else:
                    # no, read the remaining data size
                    raw_data += self.com_hdlr.send_readram_req(buff_ptr, data_to_request)
                    data_to_request = 0 # should be 0!
                    start_idx += data_to_request
                    
        raw_data += self.com_hdlr.send_readram_req(buff_ptr, data_to_request)
        logger.debug('Raw data from buffer: %s', binascii.b2a_hex(raw_data))
        
        # populate the symbols with the data
        i = 0
        while i < len(raw_data):
            for sym in self.symbols:
                fmt = self.com_hdlr.arch.get_format(sym.type)
                value = struct.unpack_from(fmt, raw_data, i)[0]
                sym.rec_buff.append(value)
                i += struct.calcsize(fmt)
    
    ## Plot the Symbol recorded data 
    #
    # Use Matplotlib to plot the recorded data for the given symbol.
    #
    # @param[in] sym The Symbol to plot
    def plot_sym(self, sym):
        # argument validation
        if not sym in self.symbols:
            raise MasterCommanderException('Symbol to plot: ' + sym.name + ' MUST be in the Recorder registered symbols!')
        
        if not hasattr(sym, 'rec_buff'):
            raise MasterCommanderException('No data to plot for ' + sym.name + ': did you call the Recorder?')
        
        plt.figure()
        plt.title(sym.name)
        plt.xlabel('Time [s]')
        plt.ylabel('Values')
        xfmt = matplotlib.ticker.ScalarFormatter(useOffset=False)
        yfmt = matplotlib.ticker.ScalarFormatter(useOffset=False)
        x = np.arange(self.timebase, (len(sym.rec_buff) + 1) * self.timebase, self.timebase)
        plt.plot(x, sym.rec_buff, 'ro')
        xax = plt.gca().xaxis
        yax = plt.gca().yaxis
        xax.set_major_formatter(xfmt)
        yax.set_major_formatter(yfmt)
        plt.show()
        
    # ----- private methods ----- #
    def _send_getsts_cmd(self):
        checksum    = 0
        sum         = 0
         
        # builds the packet
        pkt  = chr(EnumPktBytes.SOB)
        pkt += chr(EnumPktBytes.CMD_GETRECSTS)

        # checksum computation
        for c in pkt[1:]:
            sum += ord(c)
        checksum = tohex(~(sum) + 1)
        pkt += chr(checksum)

        logger.debug('Message to be sent: %s', binascii.b2a_hex(pkt))
        elab_pkt = self.com_hdlr._build_repl_sob(pkt)
        # sends the packet
        self.com_hdlr.sercom.write(elab_pkt)
        # now parse the response
        expected_len = 3
        try:
            answer = self.com_hdlr._parser(expected_len)
        except ParserException:
            logger.error('Parser Error...')
            raise
    
        return answer
    
    def _send_setup_cmd(self):
        checksum    = 0
        sum         = 0
        
        address_fmt = self.com_hdlr.arch.address_fmt + 'H'
        data_fmt    = self.com_hdlr.arch.data_fmt + 'B'
        
        # for each value to write, we have 2 bytes of address, and 1 byte of value
        msg_len = 16 + len(self.symbols) * 3
         
        # build the packet
        # fixed struct first
        pkt  = chr(EnumPktBytes.SOB)
        pkt += chr(EnumPktBytes.CMD_SETUPREC)
        pkt += chr(msg_len)
        pkt += chr(self.trigger_mode)
        pkt += struct.pack('<H', self.n_samples)
        pkt += struct.pack('<H', self.n_post_tr_samples)
        pkt += struct.pack('<H', self.time_div - 1)
        pkt += struct.pack(address_fmt, self.tr_var_addr)
        pkt += chr(self.tr_var_size)
        pkt += chr(self.tr_comp_mode)
        pkt += struct.pack('<i', self.tr_thr_val)
        pkt += chr(self.n_rec_vars)
        
        # pack addresses and values
        for s in self.symbols:
            pkt += chr(s.size)
            pkt += struct.pack(address_fmt, s.address)
        
        # checksum computation
        for c in pkt[1:]:
            sum += ord(c)
        checksum = tohex(~(sum) + 1)
        pkt += chr(checksum)

        logger.debug('Message to be sent: %s', binascii.b2a_hex(pkt))
        elab_pkt = self.com_hdlr._build_repl_sob(pkt)
        # sends the packet
        self.com_hdlr.sercom.write(elab_pkt)
        # now parse the response
        expected_len = 3
        try:
            answer = self.com_hdlr._parser(expected_len)
        except ParserException:
            logger.error('Parser Error...')
            raise
        
        return answer
    
    def _send_start_cmd(self):
        checksum    = 0
        sum         = 0
         
        # builds the packet
        pkt  = chr(EnumPktBytes.SOB)
        pkt += chr(EnumPktBytes.CMD_STARTREC)

        # checksum computation
        for c in pkt[1:]:
            sum += ord(c)
        checksum = tohex(~(sum) + 1)
        pkt += chr(checksum)

        logger.debug('Message to be sent: %s', binascii.b2a_hex(pkt))
        elab_pkt = self.com_hdlr._build_repl_sob(pkt)
        # sends the packet
        self.com_hdlr.sercom.write(elab_pkt)
        # now parse the response
        expected_len = 3
        try:
            answer = self.com_hdlr._parser(expected_len)
        except ParserException:
            logger.error('Parser Error...')
            raise
    
        return answer
    
    def _send_stop_cmd(self):
        checksum    = 0
        sum         = 0
         
        # builds the packet
        pkt  = chr(EnumPktBytes.SOB)
        pkt += chr(EnumPktBytes.CMD_STOPREC)

        # checksum computation
        for c in pkt[1:]:
            sum += ord(c)
        checksum = tohex(~(sum) + 1)
        pkt += chr(checksum)

        logger.debug('Message to be sent: %s', binascii.b2a_hex(pkt))
        elab_pkt = self.com_hdlr._build_repl_sob(pkt)
        # sends the packet
        self.com_hdlr.sercom.write(elab_pkt)
        # now parse the response
        expected_len = 3
        try:
            answer = self.com_hdlr._parser(expected_len)
        except ParserException:
            logger.error('Parser Error...')
            raise
    
        return answer
    
    def _send_getbuff_cmd(self):
        checksum    = 0
        sum         = 0
         
        # builds the packet
        pkt  = chr(EnumPktBytes.SOB)
        pkt += chr(EnumPktBytes.CMD_GETRECBUFF)

        # checksum computation
        for c in pkt[1:]:
            sum += ord(c)
        checksum = tohex(~(sum) + 1)
        pkt += chr(checksum)

        logger.debug('Message to be sent: %s', binascii.b2a_hex(pkt))
        elab_pkt = self.com_hdlr._build_repl_sob(pkt)
        # sends the packet
        self.com_hdlr.sercom.write(elab_pkt)
        # now parse the response
        expected_len = 7
        try:
            answer = self.com_hdlr._parser(expected_len)
        except ParserException:
            logger.error('Parser Error...')
            raise
    
        return answer
    
if __name__ == '__main__':
    arch = EnumArch.RENESAS_RX62T
    comport = 3
    baudrate = 57600
    sym = dwarf_parser.DwarfParser('./FOC_2.out')

    mc = MasterCommander(comport, baudrate, 0.2, arch, 35) 
    
    mc.read_board_config()
    
    sym_list = [sym.counter, sym.counter2]

    rec = Recorder(name='Prova', 
                   com_hdlr=mc, 
                   trigger_mode=0, 
                   n_samples=75,
                   n_post_tr_samples=0, 
                   time_div=1, 
                   trig_symbol=None, 
                   tr_comp_mode=0, 
                   tr_thr_val=0, 
                   symbols=sym_list)
    rec.setup()
    rec.start()
    #rec._send_getsts_cmd()
    
    # enable the isr
    sym.interrupt_master_cmd.value = 1
    mc.write_multi_sparse((sym.interrupt_master_cmd,))
    
    # activate the FOC
    sym.f_activate.value = 1
    mc.write_multi_sparse((sym.f_activate,))
    
    rec.get_data(5)
    
    rec.plot_sym(sym.counter2)
    
    for sym in rec.symbols:
        print(sym.name + ': ' + str(sym.rec_buff))