

#define FIRMWARE_VERSION	1.00f
#define DEVICE_MARKING_CODE	1010.01f
#define FIRMWARE_SUBVERSION	0.05f

//#define DEV_WITHOUT_4_20	1
//#define DEV_WITHOUT_FLOMAC	1
//#define SIMULATION			1

// define IN_4_20_MA_LOG	1

#ifdef SIMULATION

extern float simulation_mass_inventory_value;
extern float simulation_mass_rate_value;
#endif
