__author_____ = 'Ragheb Rahmaniani'
__email______ = 'Ragheb.Rahmaniani@Gmail.Com'
__copyright__ = 'All Rights Reserved.'
__revision___ = '02.01.0isr'
__update_____ = '8/11/2018'
__note_______ = 'This python scripts can be used to export master and subproblems of Multicommodity Capacitated Fixed-charge Stochastic Network Design Problem.'


import logging as logger
import os
import subprocess

logger.basicConfig(level=logger.INFO,
                   format='[%(asctime)s]: [%(levelname)s]: %(message)s')

'''
    # Specify the path to the solver (i.e., Binary of BCompose).
    # Specify the path to the 'models' folder.
    # 'models' folder should be in the same place as BCompose.
    # 'opt_model_dir' is a buffer folder for BCompose.
    # 'opt_model_dir' should be in the same place as Bcompose.
'''
path_to_BCompose = "/Users/raghebrahmaniani/BCompose_Mac_Version"
models_dir = "{0}/models".format(path_to_BCompose)
opt_model_dir = "{0}/opt_model_dir".format(path_to_BCompose)


def Cleaner():
    logger.info('Cleaning up the old stuff...')
    subprocess.call('rm' + " " + 'main', shell=True)
    subprocess.call('rm' + " " + 'main.o', shell=True)
    subprocess.call('rm' + " " + models_dir + '/*', shell=True)
    subprocess.call('rm' + " " + opt_model_dir + '/*', shell=True)
    logger.info("Cleaning Done.")


def Compiler():
    # set the directories where IloCplex's libs can be found
    CONCERTDIR = '/path/to/concert/include/'
    CPLEXDIR = '/path/to/cplex/include/'
    CPLEXLIB = '/path/to/cplex/lib/x86-64_osx/static_pic/'
    CONCERTLIB = '/path/to/concert/lib/x86-64_osx/static_pic/'
    # compiler
    GCC = 'g++'
    # set the compiler flags
    OPT_FLAG = '-Ofast'
    DEBUG_FLAG = '-DGLIBCXX_DEBUG -g -O -DIL_STD -Wall -Wextra -pedantic'
    # Pick the compiling flag
    FLAG = OPT_FLAG  # OPT_FLAG,DEBUG_FLAG
    # Compiling
    logger.info('Compiling the export program with c++17...')
    try:
        logger.info(' -Compiling docopt...')
        arg_base = '{0} {1} -std=c++17 -DNDEBUG -DIL_STD -c util/docopt/docopt.cpp  -g'.format(
            GCC, FLAG)
        subprocess.call(arg_base, shell=True)
        logger.info(' -Compiling exporter...')
        arg_base = '{0} {1} -I{2} -I{3} -std=c++17  -DIL_STD -c  main.cpp  -g'.format(
            GCC, FLAG, CPLEXDIR, CONCERTDIR)
        subprocess.call(arg_base, shell=True)
    except Exception as e:
        logger.error('Failed to compile the exporter...')
        logger.error(e)
    logger.info("Compiling Done.")
    # Linking
    logger.info("Linking the complied objects...")
    arg_base = '{0} {1} -L{2} -L{3}  -std=c++17 -o main main.o docopt.o  -ldl -lboost_system -lboost_thread-mt -lilocplex -lconcert -lcplex -lm -lpthread'.format(
        GCC, FLAG, CPLEXLIB, CONCERTLIB)
    subprocess.call(arg_base, shell=True)
    logger.info("Linking Done.")


def Exporter():
    logger.info('Setting the instances to be exported...')
    INSTANCE_LIST = ['04']  # ['04', '05', '06', '07', '08', '09', '10', '11']
    COST_CAP_RATIOS = [3]  # [3,5,7,9]
    CORRELATION = [0]  # [0,0.2,0.8]
    SCENARIOS = [16]  # [16, 32, 64]
    network_info = {
        '04': [10, 60, 10],
        '05': [10, 60, 25],
        '06': [10, 60, 50],
        '07': [10, 82, 10],
        '08': [10, 83, 25],
        '09': [10, 83, 50],
        '10': [20, 120, 40],
        '11': [20, 120, 100],
        '14': [20, 220, 100],
    }
    for instance in INSTANCE_LIST:
        num_nodes = network_info[instance][0]
        num_arcs = network_info[instance][1]
        num_od = network_info[instance][2]
        for capacity_ratio in COST_CAP_RATIOS:
            logger.info('Loading the data files...')
            instance_file_name = 'r' + instance + \
                '.' + str(capacity_ratio) + '.dow'
            logger.info('  Network file {0} is picked!'.format(
                instance_file_name))
            instance_file_path = './data_files/' + instance_file_name
            for correlation in CORRELATION:
                for num_scenario in SCENARIOS:
                    scenario_file_name = 'r' + instance + '-' + \
                        str(correlation) + '-' + str(num_scenario)
                    logger.info('  Scenario file {0} is picked!'.format(
                                scenario_file_name))
                    scenario_file_path = './data_files/scenarios/' + scenario_file_name
                    logger.info('Running the export command:  ./main --num_nodes={0}  --num_od={1} --num_arcs={2} --num_scenario={3} --file_path={4} --scenario_path={5}  --export_path={6}'.format(
                        num_nodes, num_od, num_arcs, num_scenario, instance_file_path, scenario_file_path, models_dir))
                    logger.info('Starting the export...')
                    try:
                        subprocess.call('./main --num_nodes=' +
                                        str(num_nodes) + ' --num_od=' + str(num_od) +
                                        ' --num_arcs=' +
                                        str(num_arcs) + ' --num_scenario='
                                        + str(num_scenario) + ' --file_path=' +
                                        instance_file_path + ' --scenario_path=' +
                                        scenario_file_path + ' --export_path=' + models_dir, shell=True)
                        Solver()
                    except Exception as e:
                        logger.error('Failed to export models')
                        logger.error(e)
    logger.info('Done With Exporting.')


def Solver():
    logger.info('Solving the exported problem...')
    # Go to where BCompose is
    os.chdir(path_to_BCompose)
    # Copy the models from 'models' to 'opt_model_dir'
    subprocess.call('cp' + " " + models_dir +
                    '/*.sav' + " " + opt_model_dir + '/', shell=True)
    # Run the optimization
    subprocess.call('./main  --model_dir=models' +
                    ' --current_dir={0}'.format(path_to_BCompose), shell=True)
    # Go back to the export dir
    os.chdir('examples/interface_ND')
    logger.info("Problem Solved.")


def main():
    Cleaner()
    Compiler()
    Exporter()


if __name__ == '__main__':
    main()
else:
    logger.error('This is a standalone module!')
