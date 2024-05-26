## How to compile

Open a terminal in this folder directory and execute the following command:


```bash
./compile_run.sh
```

## Set symmetric/oblivious mode

The program is able to generate symmetric or oblivious keys.
To set the mode set the input parameter `keyType` to 0 (oblivious) or 1 (symmetric) in `input_parameters_rx.txt` and `input_parameters_tx.txt`.

## Set the parity check matrix

Right now, there's two different matrices two choose from, `sparseParityCheckMatrix_n1944_k972_nonZeros6885.dat` or `sparseParityCheckMatrix_n10000_k200_nonZeros32800.dat`.

To select, edit the `parityCheckMatrixFileName` value in `input_parameters_rx.txt` and `input_parameters_tx.txt` with one of the values above.