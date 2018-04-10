#include <avr/io.h>
#include <util/delay.h>
#include <uart.h>
#include <sensor.h>
#include <moteur.h>
#include <adc.h>

int main(void) {
	DDRA = 0x00;
    uart_init();
    moteur_init();
    adc_init();
    uart_printf("\n\r");
   
    struct capteurs capteurs = CAPTEURS_INIT(0, 1);
		sensor_read(&capteurs);
        sensor_get_value(&(capteurs.gauche));
        sensor_get_value(&(capteurs.droit));

        changement_coter(&capteurs, 1);
        //moteur_avancer(128);
        //_delay_ms(3000);
        //moteur_arreter();
         
    while (1) {
		sensor_read(&capteurs);
        sensor_get_value(&(capteurs.gauche));
        sensor_get_value(&(capteurs.droit));
        moteur_ajustement(&capteurs, 0);
        
        _delay_ms(10);
    }
}
