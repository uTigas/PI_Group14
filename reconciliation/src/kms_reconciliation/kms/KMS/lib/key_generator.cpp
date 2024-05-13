#include "save_ascii_2024.h"
#include "netxpto_20200819.h"
#include <typeinfo>
#include <iostream>
#include <memory>
#include "key_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#define KEY_SIZE_BITS 36
#define SEED_SIZE_BITS 32 // Duas seeds de 32 bits cada
/* #define SIGNAL_SIZE_BITS (KEY_SIZE_BITS / 2) // Metade do tamanho da chave
 */
void KeyGenerator::initialize(void){

}

bool KeyGenerator::runBlock(){

    if (getTerminated()) return false;

    if(getFirstTime()){
        setFirstTime(false);
    } else if( seed == 0 ){
        seed = 2;
    } else { seed++;}

    // Gera a chave "oblivious"
    uint8_t tx_key[KEY_SIZE_BITS+1];
    uint8_t rx_key[KEY_SIZE_BITS+1];

    switch (Key_type)
    {
    case 0:
        
        std::cout << "seed: " << seed << "\n";

        generate_random_key(tx_key, KEY_SIZE_BITS, seed);
        
        // Adiciona a chave simetrica ao output signal
        for(int i = 0; i < KEY_SIZE_BITS; i++){
            outputSignals[0]->bufferPut((t_binary)(tx_key[i]));
        }

        for(int i = 0; i < KEY_SIZE_BITS; i++){
            outputSignals[1]->bufferPut((t_binary)(tx_key[i]));
        }

        return true;
        
    case 1:
        
        rx_seed = seed + 1;
        std::cout << "Obl.Seed TX: " << seed << "\n";
        std::cout << "Obl.Seed RX: " << rx_seed << "\n";

        generate_random_key(tx_key, KEY_SIZE_BITS, seed);
        generate_oblivious_key(tx_key, rx_key, rx_seed);


        // Adiciona a chave "oblivious" ao output signal
        for(int i = 0; i < KEY_SIZE_BITS; i++){
            outputSignals[0]->bufferPut((t_binary)(rx_key[i]));
        }

        for(int i = 0; i < KEY_SIZE_BITS; i++){
            outputSignals[1]->bufferPut((t_binary)(tx_key[i]));
        }

        // Imprime a chave "oblivious" gerada
        for (int i = 0; i < KEY_SIZE_BITS; i++) {
            printf("%d ", tx_key[i]);
        }
        printf("\n");
        
        for (int i = 0; i < KEY_SIZE_BITS; i++) {
            printf("%d ", rx_key[i]);
        }
        printf("\n");

        return true;
     
    default:
        break;
    }
   
}