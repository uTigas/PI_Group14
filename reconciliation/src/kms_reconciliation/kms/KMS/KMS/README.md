## Compiling KMS, KeyProviderServer & APPA

To compile the KMS, KeyProviderServer and APPA, use the following command:

```bash
./compile_all.sh
```

## Running the program

To run the program you need to run `./kms` and `./key_provider` in distinct terminals.
To run the app you need to run `./appA` in another terminal. It works just as the key provider but connects to the kms' north interface and requests keys. 
They can (and should) run all at the same time.

## Input Parameters

You can change the input parameters for KMS by editing `input_parameters_KMS.txt`.
You can change the input parameters for KeyProviderServer by editing `input_parameters_KeyProvider.txt`.

Each line in these files should contain a parameter name and its value, separated by an equals sign (`=`). For example:

```txt
parameter1=value1
parameter2=value2
```
Please note that these files should not contain any empty lines.
After changing the parameters, save the file and re-run (no need for re-compiling) the KMS or KeyProviderServer to use the new parameters.
