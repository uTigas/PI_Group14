## How to compile
### Tx

Edit cv_qokd_ldpc_multi_machine_sdf.cpp in order for main function to be as follow.

```c
int main(int argc, char* argv[])
{
	tx::main(argc, argv);
	//rx::main(argc, argv);

	return 0;
}
```

Compile and rename the executable file.

```bash
make
mv cv_qokd_ldpc_rx_linux cv_qokd_ldpc_tx_linux
```

### Rx

Edit cv_qokd_ldpc_multi_machine_sdf.cpp in order for main function to be as follow.

```c
int main(int argc, char* argv[])
{
	//tx::main(argc, argv);
	rx::main(argc, argv);

	return 0;
}
```
Compile.

```bash
make
```

## Execute

In two distinct terminal run the executables. Alaways start the tx first.

```bash
cv_qokd_ldpc_tx_linux
```

```bash
cv_qokd_ldpc_rx_linux
```

## Symmetric/oblivious mode set

The program is able to generate symmetric or oblivious keys.
To set the mode set the input parameter "keyType" to 0 (oblivious) or 1 (symmetric).
