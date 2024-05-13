# define PROGRAM_KEY_GENERATOR_H_

#define KEY_SIZE_BITS 36
#define SEED_SIZE_BITS 32 // Duas seeds de 32 bits cada
/* #define SIGNAL_SIZE_BITS (KEY_SIZE_BITS / 2) // Metade do tamanho da chave
 */
#include "netxpto_20200819.h"


class KeyGenerator : public Block {

public:

    void setKey_type(unsigned int key_type){Key_type = key_type;};
    unsigned int getKey_type(){return Key_type;};

    void setFirst_value(unsigned int first_value){First_value = first_value;};
    unsigned int getFirst_value(){return First_value;};

    void setLast_value(unsigned int last_value){Last_value = last_value;};
    unsigned int getLast_value(){return Last_value;};

    void setseed(unsigned int Seed){seed = Seed;};
    unsigned int getseed(){return seed;};

    KeyGenerator(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

    void initialize(void);
    bool runBlock(void);
   

    // Função para validar o tamanho da chave
    int validate_key_size(int key_size_bits) {
        return key_size_bits > 0;
    }

    // Função para gerar uma chave aleatória de 'size' bits atraves de um rng
    void generate_random_key(uint8_t *key, int size, unsigned int seed) {
        
        srand(seed);
        for (int i = 0; i < size; i++) {
            key[i] = rand() % 2;
        }
    }

    // Função para imprimir a chave em formato binário
    void print_binary_key(const uint8_t *key, int size) {
            for (int i = 0; i < size; i++) {
                printf("%d", (key[i/8] >> (7 - i%8)) & 1);
                if (i % 8 == 7) printf(" ");
            }
            printf("\n");
        }

    void generate_oblivious_key(uint8_t *tx_key, uint8_t *rx_key,unsigned int seed) {
        
        srand(seed);
        for (int i = 0; i <= KEY_SIZE_BITS; i += 2) {
            //std::cout << "i: " << i << "\n";
            uint8_t tx_bit0 = tx_key[i];
            uint8_t tx_bit1 = tx_key[i + 1];
            uint8_t rx_bit = rand() % 2; // Usa o segundo RNG para determinar se ficamos com o bit 0 ou 1 da chave do TX
            //std::cout << "rx_bit: " << rx_bit << "\n";
            // Configura os bits no rx_key de acordo com a decisão do segundo RNG
            rx_key[i] = rx_bit;
            if (rx_bit == 0) {
                rx_key[i + 1] = tx_bit0;
            }
            if (rx_bit == 1) {
                rx_key[i + 1] = tx_bit1;
            }
            //std::cout << "rx_key[i+1]: " << rx_key[i+1] << "\n";
        }
    }
private:
    t_integer ready;
    unsigned int rx_seed;
    unsigned int seed;
    unsigned int Key_type=1; // 0 - symetric, 1 - oblivious
    unsigned int First_value=0;
    unsigned int Last_value=0;
    t_string txFileName{"sym_tx"};                                //Tx files name:  keys    [[sym_tx/obl_tx.[dat/b64]] 
    t_string rxFileName{"sym_rx"};                                //Rx files name:  keys    [[sym_rx/obl_rx.[dat/b64]]  


    /* t_string Fyle_type;      // File Type: ASCII/Base64  
    unsigned int First_value=0;//Counter First Value: 0   
    unsigned int Last_value=0; //Counter Last Value: 0 */

}
;
