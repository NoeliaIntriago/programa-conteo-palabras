# Programa Conteo de Palabras
Programa basado en el manejo de memoria dinámica a través del conteo de palabras en un archivo de texto plano.

## Uso
El programa tiene el siguiente uso:
```
./wordcount -h
wordcount imprime las palabras más frecuentes en un archivo de texto.
uso:
 ./wordcount [-w <num palabras>] [-r] [<nombre archivo>]
 ./wordcount -h
Opciones:
 -h			Ayuda, muestra este mensaje
 -w <num palabras>	Especifica el número de palabras a mostrar [default: 20]
 -r			Muestra frecuencia relativa
```
El programa puede recibir la cantidad de palabras a mostrar (opcional) y el archivo de texto plano que se debe leer.
```
$ ./wordcount -w 5 sample.txt
```
También puede mostrar la frecuencia relativa de cada palabra en el archivo.
```
$ ./wordcount -w 5 -r sample.txt
```

### Ejemplo de ejecución usando -w
```
$ ./wordcount -w 5 sample.txt
me	5
con	4
jueves	4
los	4
un	3
```

### Ejemplo de ejecución usando -r
```
$ ./wordcount -w 5 -r sample.txt
me	0.049
con	0.039
jueves	0.039
los	0.039
un	0.029
```
## Compilación con make
```
$ make
```
