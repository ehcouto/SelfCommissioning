## @file self_commissioning.py
# 
# SW tool for automatic identification of BPM motor parameters

import time
import sys
import numpy as np
import os
import datetime
import logging
from gooey import Gooey
from gooey import GooeyParser
import configparser
sys.path.extend(['.', '..'])
import dwarf_parser.dwarf_parser as dwarf_parser
import master_commander.master_commander as master_commander

# log setup
logging.basicConfig()
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

REPORT_FILE_PATH = './out'

# auto calibration parameters
INJ_VOLTAGE = 5
INJ_FREQUENCY = 200
INJ_FREQUENCIES = [200, 400, 600]

class SelfCommissioningComHandler(master_commander.MasterCommander):
    def __init__(self, serial_port, serial_baud, serial_timeout, arch, mc_c_buffer_size = 35):
        super().__init__(serial_port, serial_baud, serial_timeout, arch)
        self.sym = None
        
    def reset_mcu(self):
        TIMEOUT_POWERUP = 20
        try:
            self.sym.BD_Reset.value = 2
            self.write_multi_sparse((self.sym.BD_Reset,))
        except master_commander.MasterCommanderException:
            logger.info('Waiting for reset...')
            time.sleep(2)
            # after the reset set the current limitations
     
            t0 = time.time()
            elapsed_time = time.time() - t0 
            self.read_sym_tuple((self.sym.SR_Motor_Flags,))
            while (self.sym.SR_Motor_Flags.value != 0x201 and elapsed_time < TIMEOUT_POWERUP):
                self.read_sym_tuple((self.sym.SR_Motor_Flags,))
                logger.debug('SR_Motor_Flags = %s', hex(self.sym.SR_Motor_Flags.value))
                elapsed_time = time.time() - t0 
                time.sleep(1)
            if self.sym.SR_Motor_Flags.value != 0x201:
                raise RuntimeError('IDLE condition not reached')
        
    def write_sym_tuple(self, symbol_tuple, max_attempts=5):
        retries = 0
        while retries < max_attempts:
            try:
                self.write_multi_sparse(symbol_tuple)
                # success
                retries = max_attempts
            except master_commander.MasterCommanderException:
                retries += 1
                logger.warning('Error while writing tuple: %s', str(symbol_tuple))
                logger.warning('Retries: %d', retries)
                if retries >= max_attempts:
                    raise
                
    def read_sym_tuple(self, symbol_tuple, max_attempts=5):
        retries = 0
        while retries < max_attempts:
            try:
                self.read_multi_sparse(symbol_tuple)
                # success
                retries = max_attempts
            except master_commander.MasterCommanderException:
                retries += 1
                logger.warning('Error while reading tuple: %s', str(symbol_tuple))
                logger.warning('Retries: %d', retries)
                if retries >= max_attempts:
                    raise
                
  
    def drive_motor(self, speed_integer):
        # speed command
        self.sym.Sc_Master_Cmd_Speed.value = speed_integer
        self.write_sym_tuple((self.sym.Sc_Master_Cmd_Speed,))
        self.sym.Sc_Master_Cmd_Force.value = 1
        self.write_sym_tuple((self.sym.Sc_Master_Cmd_Force,))
        
        self.read_sym_tuple((self.sym.Sc_IO_Data.Speed_Rot_Ref,))
        speed_set_point = self.sym.Sc_IO_Data.Speed_Rot_Ref.value*60/(2*np.pi)
        
        while speed_set_point < (speed_integer * 0.98):
            self.read_sym_tuple((self.sym.Sc_IO_Data.Speed_Rot_Ref,))
            speed_set_point = self.sym.Sc_IO_Data.Speed_Rot_Ref.value*60/(2*np.pi)
            # speed command
            self.sym.Sc_Master_Cmd_Speed.value = speed_integer
            self.write_sym_tuple((self.sym.Sc_Master_Cmd_Speed,))
            self.sym.Sc_Master_Cmd_Force.value = 1
            self.write_sym_tuple((self.sym.Sc_Master_Cmd_Force,))
            
    def stop_motor(self):
        pass
    
            
@Gooey(program_name='Self Commissioning Tool')
def main():
    # ---------------- ARGUMENTS PARSING ---------------- # 
    # arguments definition and parsing
    parser = GooeyParser(description='Self Commissioning tool')
    parser.add_argument('config_path', help='The configuration file path', widget='FileChooser')
    
    args = parser.parse_args()
    
    # ---------------- CONFIGURATION PARSING ---------------- # 
    print('==> Parsing the configuration file...')
    cfg_hdlr = configparser.RawConfigParser()
    try:
        cfg_hdlr.read(args.config_path)
        print('==> Done!')
    except ConfigParser.Error as e:
        raise RuntimeError('Error reading configuration file: ' + str(e))
    # parse the configuration sections
    # Project
    PROJECT_NAME = cfg_hdlr.get('Project', 'ProjectName')
    ELF_PATH = cfg_hdlr.get('Project', 'ElfPath')
    # Communication 
    COMPORT = cfg_hdlr.get('Communication', 'SerialPort')
    BAUDRATE = cfg_hdlr.getint('Communication', 'SerialBaud')
    SERIAL_TIMEOUT = cfg_hdlr.getfloat('Communication', 'SerialTimeout')
    ARCH = cfg_hdlr.get('Communication', 'SerialArch')
    BUFFER_SIZE = cfg_hdlr.getint('Communication', 'ComBufferSize')
    try:
        ARCH = master_commander.EnumArch[ARCH]
    except KeyError:
        raise RuntimeError('Wrong architecture: ' + ARCH)
    # Common
    ACQ_SAMPLES = cfg_hdlr.getint('Common', 'AcqSamples')
    INVERTER_VOLT_DROP = cfg_hdlr.getfloat('Common', 'InverterVoltDrop')
    
    # AutoCalibration
    AUTO_CALIBRATION_ENABLE = cfg_hdlr.getboolean('Common', 'EnableAutoCalibration')
    if not AUTO_CALIBRATION_ENABLE:
        # Inductance measurement
        frequency = [int(i.strip()) for i in cfg_hdlr.get('Inductance', 'Frequency').split(',')]
        voltage_min = [float(i.strip()) for i in cfg_hdlr.get('Inductance', 'VoltageMin').split(',')]
        voltage_max = [float(i.strip()) for i in cfg_hdlr.get('Inductance', 'VoltageMax').split(',')]
        n_points = [int(i.strip()) for i in cfg_hdlr.get('Inductance', 'VoltagePoints').split(',')]
        l = []
        for i_min, i_max, freq, n_points in zip(voltage_min, voltage_max, frequency, n_points):
            currents = np.linspace(i_min, i_max, n_points)
            f = np.ones(len(currents)) * freq
            l.append(zip(currents, f))
        INJ_INPUT_AC_VOLT_FREQ = [item for sublist in l for item in sublist]
        # Resistance measurement
        current_dc_min = [float(i.strip()) for i in cfg_hdlr.get('Resistance', 'CurrentDCMin').split(',')]
        current_dc_max = [float(i.strip()) for i in cfg_hdlr.get('Resistance', 'CurrentDCMax').split(',')]
        n_points = [int(i.strip()) for i in cfg_hdlr.get('Resistance', 'CurrentDCPoints').split(',')]
        l = []
        for i_min, i_max, n_points in zip(current_dc_min, current_dc_max, n_points):
            currents = np.linspace(i_min, i_max, n_points)
            f = np.zeros(len(currents))
            l.append(zip(currents, f))
        INJ_INPUT_DC_VOLT_FREQ = [item for sublist in l for item in sublist]
        
    # Speed BEMF measurement
    speed_min = cfg_hdlr.getint('SpeedBEMF', 'SpeedMin')
    speed_max = cfg_hdlr.getint('SpeedBEMF', 'SpeedMax')
    n_points = cfg_hdlr.getint('SpeedBEMF', 'SpeedPoints')
    SPEED_BEMF = np.linspace(speed_min, speed_max, n_points)
    
    # ---------------- REPORT FILE CREATION ---------------- # 
    REPORT_FILE_PATH = './out'
    LOG_FILE_PATH = './out'
    if not os.path.exists(REPORT_FILE_PATH):
        os.makedirs(REPORT_FILE_PATH)
    
    now = datetime.datetime.strftime(datetime.datetime.now(), '%Y%m%d_%H%M%S')
    REPORT_FILE_NAME = 'SelfCommissioning_' + PROJECT_NAME + '_Report_' + now + '.txt'
    REPORT_FILE_PATH = os.path.join(REPORT_FILE_PATH, REPORT_FILE_NAME)
    # Report file creation
    with open(REPORT_FILE_PATH, 'w') as report_file:
        report_file.write('Motor Identification Parameter for Brushless PMSM Motor\n')
        report_file.write(str(PROJECT_NAME) + '\n')
        report_file.write('Date: ' + str(now))
        report_file.write('\n\n')
        
    # log file creation
    LOG_FILE_NAME = 'SelfCommissioning_' + PROJECT_NAME + '_Log_' + now + '.log'
    fh = logging.FileHandler(os.path.join(LOG_FILE_PATH, LOG_FILE_NAME))
    fh.setLevel(logging.DEBUG)
    # add the handler to the logger
    logger.addHandler(fh)
    
    # ---------------- COMMUNICATION SETUP ---------------- # 
    sym = dwarf_parser.DwarfParser(ELF_PATH)
     
    mc = SelfCommissioningComHandler(COMPORT, 
                                     BAUDRATE, 
                                     SERIAL_TIMEOUT, 
                                     arch=ARCH)
    
    mc.sym = sym
    
    # reset the DSP
    mc.reset_mcu()
    print('==> Opened communication with the board')
    
    mc.read_sym_tuple((sym.Sc_Prm.Ts,sym.Sc_Prm.Pole_Pairs))
    POLE_PAIRS = sym.Sc_Prm.Pole_Pairs.value
    Ts_Pwm = sym.Sc_Prm.Ts.value
    

    INDUCTANCE_SAMPLES = len(INJ_INPUT_AC_VOLT_FREQ)
    RESISTANCE_SAMPLES = len(INJ_INPUT_DC_VOLT_FREQ)
    SPEED_SAMPLES = len(SPEED_BEMF)
    
    # Input parameters conversions
    SPEED_BEMF_INPUT = {'Speed': np.array(SPEED_BEMF),
                        'speed_integer': np.array(SPEED_BEMF)}

    inj_input_ac_voltage, inj_input_ac_freq = zip(*INJ_INPUT_AC_VOLT_FREQ)
    inj_input_dc_current, inj_input_dc_freq = zip(*INJ_INPUT_DC_VOLT_FREQ)
    
    INJ_INPUT_AC = {'Voltage': np.array(inj_input_ac_voltage),
                    'Frequency': np.array(inj_input_ac_freq)}
    
    INJ_INPUT_DC = {'Current': np.array(inj_input_dc_current),
                    'Frequency': np.array(inj_input_dc_freq)}
    
    
    with open(REPORT_FILE_PATH, 'a') as report_file:
        report_file.write('Rs Measure\n')
        report_file.write('\n')
        report_file.write('Current [A]\t Frequency [Hz]\t Voltage [V]\t Rs [Ohm] \n')
    
    # ---------------- RESISTANCE MEASUREMENT ---------------- # 
    # Rs Measure
    rs_acc = 0
    v_array = np.zeros(RESISTANCE_SAMPLES)
    current_array = np.zeros(RESISTANCE_SAMPLES)
    rs_temp_array = np.zeros(RESISTANCE_SAMPLES)
    
    actual_samples = 0
    print('***** Rs measurement *****')
    for i in range(RESISTANCE_SAMPLES):
        logger.info('Step %d', i)
        logger.info('Current: %f', INJ_INPUT_DC['Current'][i])
        logger.info('Frequency: %f', INJ_INPUT_DC['Frequency'][i])
        
        sym.Sc_Current_Inj_Ampl.value = INJ_INPUT_DC['Current'][i]
        sym.Sc_FrequencyInj.value = int(INJ_INPUT_DC['Frequency'][i])
        
        # Ramp up to the desired current
        mc.read_sym_tuple((sym.Sc_Current_Inj_Ampl,))
        ramp_current = np.linspace(sym.Sc_Current_Inj_Ampl.value, INJ_INPUT_DC['Current'][i], 100)
        for curr in ramp_current:
            # write the injection parameters
            sym.Sc_Current_Inj_Ampl.value = curr
            sym.Sc_FrequencyInj.value = int(INJ_INPUT_DC['Frequency'][i])
            mc.write_sym_tuple((sym.Sc_Current_Inj_Ampl, sym.Sc_FrequencyInj))
            # enable the injection
            sym.Sc_Injection_Flag.value = 1
            mc.write_sym_tuple((sym.Sc_Injection_Flag,))
        
        mc.write_sym_tuple((sym.Sc_Current_Inj_Ampl,
                            sym.Sc_FrequencyInj))
        
        # enable the injection
        sym.Sc_Injection_Flag.value = 1
        mc.write_sym_tuple((sym.Sc_Injection_Flag,))
        
        # waiting for a stable response
        time.sleep(2)
        # acquisition
        volt_sample = 0
        v_ampl_acc = []
        current_acc = []
        while volt_sample < ACQ_SAMPLES:
            mc.read_sym_tuple((sym.Sc_VAmpl,sym.Sc_IAmpl))
            v_ampl_value = sym.Sc_VAmpl.value 
            current_value = sym.Sc_IAmpl.value
            logger.debug('Sc_IAmpl = ' + str(current_value))
            logger.debug('Sc_VAmpl = ' + str(v_ampl_value))
            # check for null values
            if (v_ampl_value != 0):
                v_ampl_acc.append(v_ampl_value)
            else:
                logger.warning('Zero value for Sc_VAmpl found! Discarding...')
                
                  
            if (current_value != 0):
                current_acc.append(current_value)
            else:
                logger.warning('Zero value for Sc_IAmpl found! Discarding...')    
            volt_sample += 1
            time.sleep(0.5)
            
        v_array[i] = np.mean(v_ampl_acc)
        current_array[i] = np.mean(current_acc)
        
        if v_array[i] != 0:
            # Rs calculation
            rs_temp = (v_array[i] - INVERTER_VOLT_DROP) /current_array[i] # VAmplDc/Sc_IAmpl
            logger.debug('Rs[' + str(i) + '] = '  + str(rs_temp))
        
        rs_acc += rs_temp
        
        rs_temp_array[i] = rs_temp
        actual_samples += 1
        
        with open(REPORT_FILE_PATH, 'a') as report_file:
            report_file.write(str(current_array[i]) + '\t' + str(INJ_INPUT_DC['Frequency'][i]) + '\t' + str(v_array[i]) + '\t' + str(rs_temp_array[i]) + '\n')
    
    rs_mean = rs_acc / (actual_samples)
    rs_value = rs_mean
    print('==> Rs = ' + str(rs_mean))
    with open(REPORT_FILE_PATH, 'a') as report_file:
        report_file.write('\n')
        report_file.write('RsMean [Ohm]\n')
        report_file.write(str(rs_mean) + '\n')
        report_file.write('\n\n')
        report_file.write('Ld Lq Measure\n')
        report_file.write('VoltageInjection(V) \tFrequency(Hz) \tImin(A) \tIMax(A) \tLd(H) \tLq(H)\n')
    
    # ---------------- INDUCTANCE MEASUREMENT ---------------- # 
    ld_acc = 0
    lq_acc = 0
    
    v_min_array = np.zeros(INDUCTANCE_SAMPLES)
    v_max_array = np.zeros(INDUCTANCE_SAMPLES)
    i_min_array = np.zeros(INDUCTANCE_SAMPLES)
    i_max_array = np.zeros(INDUCTANCE_SAMPLES)
    voltage_array = np.zeros(INDUCTANCE_SAMPLES)
    current_array = np.zeros(INDUCTANCE_SAMPLES)
    ld_temp_arr = np.zeros(INDUCTANCE_SAMPLES)
    lq_temp_arr = np.zeros(INDUCTANCE_SAMPLES)
    
    print('***** Inductance measurement *****')
    for i in range(INDUCTANCE_SAMPLES):
        logger.info('Step %d', i)
        logger.info('Voltage: %f', INJ_INPUT_AC['Voltage'][i])
        logger.info('Frequency: %f', INJ_INPUT_AC['Frequency'][i])
        # Ramp up to the desired voltage
        mc.read_sym_tuple((sym.Sc_VoltageInj,))
        ramp_current = np.linspace(sym.Sc_VoltageInj.value, INJ_INPUT_AC['Voltage'][i], 100)
        for curr in ramp_current:
            # write the injection parameters
            sym.Sc_VoltageInj.value = curr
            sym.Sc_FrequencyInj.value = int(INJ_INPUT_AC['Frequency'][i])
            mc.write_sym_tuple((sym.Sc_VoltageInj, sym.Sc_FrequencyInj))
            # enable the injection
            sym.Sc_Injection_Flag.value = 2
            mc.write_sym_tuple((sym.Sc_Injection_Flag,))
            
        # write the injection parameters
        sym.Sc_VoltageInj.value = INJ_INPUT_AC['Voltage'][i]
        sym.Sc_FrequencyInj.value = int(INJ_INPUT_AC['Frequency'][i])
        mc.write_sym_tuple((sym.Sc_VoltageInj, sym.Sc_FrequencyInj))
        # enable the injection
        sym.Sc_Injection_Flag.value = 2
        mc.write_sym_tuple((sym.Sc_Injection_Flag,))
        
        # waiting for a stable response
        time.sleep(2)
        # acquisition
        volt_sample = 0
        i_min_acc = []
        i_max_acc = []
        voltage_acc = []
        while volt_sample < ACQ_SAMPLES:
            mc.read_sym_tuple((sym.Sc_IAmpl_Max, sym.Sc_IAmpl_Min,sym.Sc_VAmpl))
            i_min_value = sym.Sc_IAmpl_Min.value
            i_max_value = sym.Sc_IAmpl_Max.value
            voltage_value = sym.Sc_VAmpl.value
            # check for null values
            if (i_max_value != 0):
                i_max_acc.append(i_max_value)
            else:
                logger.warning('Zero value for Sc_IAmpl_Max found! Discarding...')
            if (i_min_value != 0):
                i_min_acc.append(i_min_value)
            else:
                logger.warning('Zero value for Sc_IAmpl_Min found! Discarding...')
            
            if (voltage_value != 0):
                voltage_acc.append(voltage_value)
            else:
                logger.warning('Zero value for Sc_VAmpl found! Discarding...')    
                
            volt_sample += 1
            time.sleep(0.5)
            
        i_min_array[i] = np.mean(i_min_acc)
        i_max_array[i] = np.mean(i_max_acc)
        voltage_array[i] = np.mean(voltage_acc)
            
        # Ld Lq calculation
        # ld_temp = (voltage_array[i] - INVERTER_VOLT_DROP)/ (2 * np.pi * INJ_INPUT_AC['Frequency'][i] * i_max_array[i])
        # lq_temp = (voltage_array[i] - INVERTER_VOLT_DROP)/ (2 * np.pi * INJ_INPUT_AC['Frequency'][i] * i_min_array[i])
        # ld_temp = np.sqrt((voltage_array[i] - INVERTER_VOLT_DROP)**2/ i_max_array[i]**2 - rs_value**2) / (2 * np.pi * INJ_INPUT_AC['Frequency'][i])
        # lq_temp = np.sqrt((voltage_array[i] - INVERTER_VOLT_DROP)**2/ i_min_array[i]**2 - rs_value**2) / (2 * np.pi * INJ_INPUT_AC['Frequency'][i])
        ld_temp = np.sqrt((voltage_array[i])**2/ i_max_array[i]**2 - rs_value**2) / (2 * np.pi * INJ_INPUT_AC['Frequency'][i])
        lq_temp = np.sqrt((voltage_array[i])**2/ i_min_array[i]**2 - rs_value**2) / (2 * np.pi * INJ_INPUT_AC['Frequency'][i])
        
        ld_acc += ld_temp
        lq_acc += lq_temp
        
        ld_temp_arr[i] = ld_temp
        lq_temp_arr[i] = lq_temp
        
        with open(REPORT_FILE_PATH, 'a') as report_file:
            report_file.write(str(voltage_array[i]) + '\t' + str(INJ_INPUT_AC['Frequency'][i]) + '\t' + str(i_min_array[i]) + '\t' + str(i_max_array[i]) + '\t' + str(ld_temp_arr[i]) + '\t' + str(lq_temp_arr[i]) + '\n')
            

    ld_mean = ld_acc / (INDUCTANCE_SAMPLES)
    lq_mean = lq_acc / (INDUCTANCE_SAMPLES)
    ls_mean = (ld_mean + lq_mean) / 2.0
    
    ld_value = ld_mean
    lq_value = lq_mean
    print('==> Ld Mean = ' + str(ld_mean))
    print('==> Lq Mean = ' + str(lq_mean))
    print('==> Ls Mean = ' + str(ls_mean))
    
    with open(REPORT_FILE_PATH, 'a') as report_file:
        report_file.write('\n')
        report_file.write('LdMean [H]\t LqMean [H]\t LsMean [H]\n')
        report_file.write(str(ld_mean) + '\t' + str(lq_mean) + '\t' + str(ls_mean) + '\n')
        report_file.write('\n\n')
        report_file.write('Phim Measure\n')
        report_file.write('\n')
        report_file.write('Target Speed [Rpm]\t Speed [Rpm]\t Bemf [V]\t we [Rad/s_el]\t Phim\n')
        
    # injection disable
    sym.Sc_Injection_Flag.value = 0
    mc.write_sym_tuple((sym.Sc_Injection_Flag,))
    
    # reset MCU
    mc.reset_mcu()
    print('==> Inductance measurement DONE')
    
    # ---------------- BEMF OBSERVER PARAMETERS MEASUREMENT ---------------- # 
    # Waiting for a stable response
    time.sleep(5)
        
    # DQ observer parameters calculation
    Icoeff =         ld_value / (ld_value + Ts_Pwm * rs_value)
    Ucoeff =           Ts_Pwm / (ld_value + Ts_Pwm * rs_value)
    WIcoeff=  lq_value*Ts_Pwm / (ld_value + Ts_Pwm * rs_value) 
    Ecoeff =           Ts_Pwm / (ld_value + Ts_Pwm * rs_value)
    
    # Set the BEMF observer parameters
    sym.Sc_Bemf_Observer_Params.I_Coeff.value = Icoeff
    sym.Sc_Bemf_Observer_Params.U_Coeff.value = Ucoeff
    sym.Sc_Bemf_Observer_Params.WI_Coeff.value = WIcoeff
    sym.Sc_Bemf_Observer_Params.E_Coeff.value = Ecoeff
    
    mc.write_sym_tuple((sym.Sc_Bemf_Observer_Params.I_Coeff,
                    sym.Sc_Bemf_Observer_Params.U_Coeff,
                    sym.Sc_Bemf_Observer_Params.WI_Coeff,
                    sym.Sc_Bemf_Observer_Params.E_Coeff))
    
    print('Icoeff = ', Icoeff)
    print('Ucoeff = ', Ucoeff)
    print('WIcoeff = ', WIcoeff)
    print('Ecoeff = ', Ecoeff)
    
      # Set the BEMF observer parameters
    sym.Sc_Prm.Rs.value = rs_value
    sym.Sc_Prm.Ld.value = ld_value
    sym.Sc_Prm.Lq.value = lq_value
    
    
    mc.write_sym_tuple((sym.Sc_Prm.Rs,
                        sym.Sc_Prm.Ld,
                        sym.Sc_Prm.Lq))
    
    
    # Phim measurement
    phim_acc = []
    read_omega_array = np.zeros(SPEED_SAMPLES)
    read_bemf_array = np.zeros(SPEED_SAMPLES)
    we_array = np.zeros(SPEED_SAMPLES)
    phim_temp_array = np.zeros(SPEED_SAMPLES)
    
    print('***** Phi_m measurement *****')
    for i in range(SPEED_SAMPLES):
        logger.info('Step %d', i)
        logger.info('Speed: %f', SPEED_BEMF_INPUT['Speed'][i])
        # speed command
        mc.drive_motor(int(SPEED_BEMF_INPUT['speed_integer'][i]))
        # waiting for a stable response
        time.sleep(4)
        
        # acquisition
        curr_sample = 0
        read_omega_acc = []
        read_bemf_acc = []
        while curr_sample < ACQ_SAMPLES:
            mc.read_sym_tuple((sym.Sc_Data.Speed_Rot_Est_El_Abs, 
                               sym.Sc_Bemf_Ampl))
            read_omega_value = sym.Sc_Data.Speed_Rot_Est_El_Abs.value
            read_bemf_value = sym.Sc_Bemf_Ampl.value 
            # check for null values
            if (read_omega_value != 0):
                read_omega_acc.append(read_omega_value)
            else:
                logger.warning('Zero value for OmegaEst found! Discarding...')
            if (read_bemf_value != 0):
                read_bemf_acc.append(read_bemf_value)
            else:
                logger.warning('Zero value for Bemf_Ampl found! Discarding...')
            curr_sample += 1
            time.sleep(0.5)
            
        read_omega_array[i] = np.mean(read_omega_acc)
        read_bemf_array[i] = np.mean(read_bemf_acc)
        we_array[i] =  read_omega_array[i] 
        
        read_omega_array[i] =  read_omega_array[i] *60/(2*np.pi)/POLE_PAIRS
        
        # Phim calculation
        phim_temp = read_bemf_array[i] / we_array[i]
        phim_acc.append(phim_temp)
        phim_temp_array[i] = phim_temp
        
        # report 
        with open(REPORT_FILE_PATH, 'a') as report_file:
            report_file.write(str(SPEED_BEMF_INPUT['Speed'][i]) + '\t' + str(read_omega_array[i]) + '\t' + str(read_bemf_array[i]) + '\t' + str(we_array[i]) + '\t' + str(phim_temp_array[i]) + '\n')
        
    phim_mean = np.mean(phim_acc)
    
    phim_value = phim_mean
    k_torque_value = 1.5 * POLE_PAIRS * phim_value
    k_deflux_value = 1.5 * POLE_PAIRS * (ld_value - lq_value)
    print('==> Phi_m = ' + str(phim_mean))
    print('==> K_torque = ' + str(k_torque_value))
    print('==> K_deflux = ' + str(k_deflux_value))
    
    with open(REPORT_FILE_PATH, 'a') as report_file:
        report_file.write('\n')
        report_file.write('Phim [V/rad/s]\n')
        report_file.write(str(phim_mean) + '\n')
        report_file.write('\n\n')
        report_file.write('KTorque [Nm/A]\t' + str(k_torque_value) + '\n')
        report_file.write('KDeflux [Nm/A^2]\t' + str(k_deflux_value) + '\n')
        report_file.write('\n')
        
    # reset MCU    
    mc.reset_mcu()
    print('==> Phi_m measurement DONE *****')
    
    print('***** Test Finished *****')
    
    
if __name__ == '__main__':
    main()