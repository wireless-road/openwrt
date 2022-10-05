#include "config.h"

#ifdef SIMULATION

int simulation_valve_opened_flag = 0;

float simulation_mass_inventory_value = 0.00;

float simulate_mass_inventory_value() {

	if(simulation_valve_opened_flag) {
		simulation_mass_inventory_value += 0.01;
	}
	return simulation_mass_inventory_value;
}

void simulation_open_valve() {
	simulation_valve_opened_flag = 1;
}

#endif
