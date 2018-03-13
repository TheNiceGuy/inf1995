#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <timer.h>

/* valeur par défaut du prescaler */
#ifndef TIMER_PRESCALER
    #define TIMER_PRESCALER 1024
#endif

/** Le temps maximal d'un timer 16-bits en millisecondes. */
#define MAX_TIME_MILLISECONDS 1000*((0xFF * (uint32_t) TIMER_PRESCALER)/F_CPU)

/** Cette structure peut être utilisée pour ne pas affecter un canal. */
const struct callback CALLBACK_IGNORE = {
    .time = 0,
    .func = NULL,
};

/** Le callback pour le canal A du timer. */
void (*callbackA)(void);

/** Le callback pour le canal B du timer. */
void (*callbackB)(void);

/**
 * Cette fonction configure une interruption pour le canal A. Si la structure
 * pour le callback est `NULL`, alors l'interruption est totalement désactiver.
 * Si le temps du callback est de zéro, alors on ne touche pas au callback
 * actuel. Sinon, on remplace le callback actuel du canal B par celui dans la
 * structure.
 *
 * @param func Une pointeur vers le pointeur de callback actuel.
 * @param c    Un pointer vers une structure de callback.
 */
void configure_interruptA(void (**func)(void), struct callback* c);

/**
 * Cette fonction configure une interruption pour le canal B. Si la structure
 * pour le callback est `NULL`, alors l'interruption est totalement désactiver.
 * Si le temps du callback est de zéro, alors on ne touche pas au callback
 * actuel. Sinon, on remplace le callback actuel du canal B par celui dans la
 * structure.
 *
 * @param func Une pointeur vers le pointeur de callback actuel.
 * @param c    Un pointer vers une structure de callback.
 */
void configure_interruptB(void (**func)(void), struct callback* c);

void timer_init(void) {
    /* on veut que le compteur soit déconnecté de sa pin */
    TCCR1A = 0;

    /* clear timer on compare */
    TCCR1B = _BV(WGM12);

    /* on configure le prescaler */
#if   TIMER_PRESCALER == 1
    TCCR1B |= _BV(CS10);
#elif TIMER_PRESCALER == 8
    TCCR1B |= _BV(CS11);
#elif TIMER_PRESCALER == 64
    TCCR1B |= _BV(CS11) | _BV(CS10);
#elif TIMER_PRESCALER == 256
    TCCR1B |= _BV(CS12);
#elif TIMER_PRESCALER == 1024
    TCCR1B |= _BV(CS12) | _BV(CS10);
#else
    #error "Invalid timer prescaler value"
#endif

    /* ce registre n'est pas utilisé */
    TCCR1C = 0;
}

int8_t timer_start(struct callback* cA, struct callback* cB) {
    /* on s'assure que le temps d'attente ne dépasse pas la limite du timer */
    if(MAX_TIME_MILLISECONDS < cA->time) return INVALID_TIMEA;
    if(MAX_TIME_MILLISECONDS < cB->time) return INVALID_TIMEB;

    /* on désactive les interruptions */
    cli();

    /* on configure les interruptions */
    configure_interruptA(&callbackA, cA);
    configure_interruptB(&callbackB, cB);

    /* on démarre le compteur à zéro */
    TCNT1 = 0;

    /* on active les interruptions */
    sei();

    return 0;
}

/** Le vecteur d'interruption pour le canal A. */
ISR(TIMER1_COMPA_vect) {
    cli();
    (*callbackA)();
    sei();
}

/** Le vecteur d'interruption pour le canal B. */
ISR(TIMER1_COMPB_vect) {
    cli();
    (*callbackB)();
    sei();
}

/** Cette macro génère les fonctions pour configurer les interruptions. */
#define conf_interrupt(channel)                                              \
void configure_interrupt##channel(void (**func)(void), struct callback* c) { \
    /* on enlève le callback si aucune est spécifié */                       \
    if(c == NULL) {                                                          \
        TIMSK1 &= ~_BV(OCIE1##channel);                                      \
        (*func) = NULL;                                                      \
        return;                                                              \
    }                                                                        \
                                                                             \
    /* on ne touche pas au callback */                                       \
    if(c->time == 0) {                                                       \
        OCR1##channel -= TCNT1;                                              \
        return;                                                              \
    }                                                                        \
                                                                             \
    /* on change le callback */                                              \
    (*func) = c->func;                                                       \
    TIMSK1 |= _BV(OCIE1##channel);                                           \
    OCR1##channel = (c->time*(F_CPU/1000))/TIMER_PRESCALER;                  \
}

/* on génère les fonctions */
conf_interrupt(A)
conf_interrupt(B)