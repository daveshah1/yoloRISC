#include <iostream>
#include "VTestHarness.h"
#include <verilated_vcd_c.h>

#define MAX_TIME 10000000

// Simulation time:
vluint64_t t_sim;

// provide method to be called by $time in Verilog:
double sc_time_stamp() { return t_sim; }

int main(int argc, char *argv[])
{
	// Initialize Verilator:
	Verilated::commandArgs(argc, argv);
	VerilatedVcdC	*m_trace;
	Verilated::traceEverOn(true);
	// Instantiate DUT:
	VTestHarness *dut = new VTestHarness;

	// Initial input signals:
	dut->clock = 0;
	dut->reset = 1;
	dut->uart_rx = 0;

	m_trace = new VerilatedVcdC;
	dut->trace(m_trace, 99);
	m_trace->open("tb.vcd");

	// keep track of LED output value
	int led = 0;

	// Tick the clock for a few cycles (or until done):
	for (t_sim = 0; !Verilated::gotFinish() && t_sim < MAX_TIME; t_sim++) {
		if (t_sim > 100)       dut->reset = 0;
		if ((t_sim % 10) == 1) dut->clock = 1;
		if ((t_sim % 10) == 6) dut->clock = 0;

		dut->eval();
		m_trace->dump(t_sim);
		if ((t_sim % 100000) == 0)
			std::cout << "### +10k cycles" << std::endl;

		if (led != dut->led) {
			led = dut->led;
			std::cout << "### LED: " << led << std::endl;
		}
	}

	m_trace->close();
	m_trace = NULL;

	// Clean up:
	dut->final();
	delete dut;

	return 0;
}
