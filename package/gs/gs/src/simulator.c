#include "config.h"

#ifdef SIMULATION

int simulation_valve_opened_flag = 0;

float simulation_mass_inventory_value = 0.00;
float simulation_mass_rate_value = 0.00;

float simulate_mass_inventory_value() {

	if(simulation_valve_opened_flag) {
		simulation_mass_inventory_value += 0.01;
//		simulation_mass_rate_value -= 0.01; // Заправка 1 литра по 1-линейной схеме
		simulation_mass_rate_value -= 0.025; // Заправка 1 литра по 2-линейной схеме
//		simulation_mass_rate_value -= 0.055; // Заправка 1 литра по 3-линейной схеме
//		simulation_mass_rate_value -= 0.075; // Заправка 1 литра не успевает произойти. Происходит заправка до полного на третьей линии.
//		simulation_mass_rate_value -= 0.090; // Заправка 1 литра не успевает произойти. Происходит заправка до полного на третьей линии.
	}
	return simulation_mass_inventory_value;
}

void simulation_open_valve() {
	simulation_valve_opened_flag = 1;
	simulation_mass_rate_value = 0.91;
}

void simulation_close_valve() {
	simulation_valve_opened_flag = 0;
	simulation_mass_rate_value = 0.0;
}

void simulation_open_high_valve() {
//	simulation_valve_opened_flag = 1;
	simulation_mass_rate_value += 0.91;
}

void simulation_close_high_valve() {
//	simulation_valve_opened_flag = 0;
//	simulation_mass_rate_value = 0.0;
}

#endif
