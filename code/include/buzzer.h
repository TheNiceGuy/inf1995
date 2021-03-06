 #ifndef INCLUDE_BUZZER_H
#define INCLUDE_BUZZER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* macros pour la pin de sortie du son */
#ifndef SON_OUT_PIN
    #define SON_OUT_PIN OC2A_PIN
#endif

#ifndef SON_OUT_BIT
    #define SON_OUT_BIT OC2A_BIT
#endif

#ifndef SON_OUT_DDR
    #define SON_OUT_DDR OC2A_DDR
#endif

#ifndef SON_OUT_PORT
    #define SON_OUT_PORT OC2A_PORT
#endif

/* macros pour la pin de ground du son */
#ifndef SON_GROUND_PIN
    #define SON_GROUND_PIN PIND
#endif

#ifndef SON_GROUND_BIT
    #define SON_GROUND_BIT 6
#endif

#ifndef SON_GROUND_DDR
    #define SON_GROUND_DDR DDRD
#endif

#ifndef SON_GROUND_PORT
    #define SON_GROUND_PORT PORTD
#endif

/**
 * Cette méthode initialise le piezoelectrique.
 */
void buzzer_init(void);

/**
 * Cette méthode fait jouer un son à 880 Hz.
 */
void buzzer_jouer(void);

/**
 * Cette méthode arrête le piezoelectrique.
 */
void buzzer_arreter(void);


#ifdef __cplusplus
}
#endif

#endif
