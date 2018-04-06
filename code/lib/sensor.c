#include <avr/io.h>
#include <adc.h>
#include <sensor.h>
#include <sensor_data.h>
#include <util/delay.h>
#include <timer.h>
#include <son.h>
#include <utils.h>

#define POTEAU 1000

/** La table des distances. */
int16_t sensor_tableau[] = SENSOR_DATA;

void sensor_read(struct capteurs* capteurs) {
    /* on lit les deux capteurs */
    capteurs->gauche.raw = adc_read(capteurs->gauche.pin);
    capteurs->droit.raw  = adc_read(capteurs->droit.pin);
}

uint8_t sensor_get_value(struct capteur* capteur) {
	/* on s'assure que les valeurs lues sont dans la table */
    if(capteur->raw < SENSOR_MIN || capteur->raw > SENSOR_MAX) return FAIL;

	/* on calcule l'offset dans la table */
	uint16_t offset = capteur->raw - SENSOR_MIN;
    
    /* on obtient la distance à partir de la table */
    capteur->value = sensor_tableau[offset];

    /* la conversion s'est bien effectuée */
    return OK;
}

void son_poteau(void) {   
	son_jouer(77); 
	_delay_ms(200); 
	son_arreter();
	_delay_ms(100);
}

uint8_t sensor_is_nouveau_mur (struct capteurs* capteurs, uint8_t direction) {
	if (direction == 0) {
		while (capteurs->droit.value != 0) {
			_delay_ms(POTEAU);
			if (capteurs->droit.value != 0) {
				return 1;
			} else {
				son_poteau();
				son_poteau();
				son_poteau();
				return 0;
			}

            sensor_read(capteurs);
            sensor_get_value(&(capteurs->gauche));
            sensor_get_value(&(capteurs->droit));
		}
	} else {
		while (capteurs->gauche.value != 0) {
			_delay_ms(POTEAU);
			if (capteurs->gauche.value != 0)
				return 1;
			else {
				son_poteau();
				son_poteau();
				son_poteau();
				return 0;
			}

            sensor_read(capteurs);
            sensor_get_value(&(capteurs->gauche));
            sensor_get_value(&(capteurs->droit));
		}
	}
}
