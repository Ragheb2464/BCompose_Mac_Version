#!/bin/env python

__author_____ = 'Ragheb Rahmaniani'
__email______ = 'Ragheb.Rahmaniani@Gmail.Com'
__copyright__ = 'All Rights Reserved.'
__revision___ = '02.01.0isr'
__update_____ = '8/11/2018'
__usage______ = 'python run_me.py'
__note_______ = 'This python scripts can be used to run the algorithm.'


import logging as logger
import subprocess

# logger.basicConfig(filename='solver.log', level=logger.INFO,
#                    format='%(asctime)s:%(levelname)s:%(message)s')
logger.basicConfig(level=logger.INFO,
                   format='[%(asctime)s]: [%(levelname)s]: %(message)s')

model_files_path = "models"
pwd = "/Users/raghebrahmaniani/BCompose_Mac_Version"


subprocess.call('rm' + " " + pwd + '/opt_model_dir/*', shell=True)
subprocess.call('cp' + " " + model_files_path +
                '/*.sav' + " " + pwd + '/opt_model_dir/', shell=True)

try:
    logger.info(
        'Running the optimization command: ./main  --model_dir={0} --current_dir={1}'.format(model_files_path, pwd))
    logger.info('Starting the optimization...')
    subprocess.call('./main  --model_dir=' + model_files_path +
                    ' --current_dir=' + pwd, shell=True)
    logger.info('Finished the optimization.')
except Exception as e:
    logger.error(e)
    logger.info("Optimization failed!")
