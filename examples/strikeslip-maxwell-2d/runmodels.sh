#!/bin/bash
pylith step01_slip_elastic.cfg 2>&1 | tee step01_slip_elastic.log
pylith step02_slip_maxwell.cfg 2>&1 | tee step02_slip_maxwell.log
pylith step03_slip_maxwell.cfg 2>&1 | tee step03_slip_maxwell.log
pylith step04_slip_elastic.cfg 2>&1 | tee step04_slip_elastic.log
pylith step05_slip_maxwell.cfg 2>&1 | tee step05_slip_maxwell.log
pylith step06_slip_maxwell.cfg 2>&1 | tee step06_slip_maxwell.log
pylith step07_slip_maxwell.cfg 2>&1 | tee step07_slip_maxwell.log
pylith step08_slip_maxwell.cfg 2>&1 | tee step08_slip_maxwell.log
pylith step09_slip_maxwell.cfg 2>&1 | tee step09_slip_maxwell.log
pylith step10_slip_maxwell.cfg 2>&1 | tee step10_slip_maxwell.log
pylith step11_slip_maxwell.cfg 2>&1 | tee step11_slip_maxwell.log
pylith step12_slip_maxwell.cfg 2>&1 | tee step12_slip_maxwell.log
