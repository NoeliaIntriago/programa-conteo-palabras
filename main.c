#include <stdio.h>   // Libreria para manipulacion de archivos
#include <getopt.h>  // Libreria para procesar los argumentos pasados en la terminal
#include <stdbool.h> // Libreria para uso de booleanos
#include <errno.h>   // Libreria para manejo de errores
#include <string.h>  // Libreria para manipulacion de cadenas de caracteres
#include <ctype.h>   // Libreria para estados de una variable
#include <stdlib.h>  // Libreria para conversiones y gestion de memoria
#define MAX_BUFFER_SIZE 512

bool rflag = false; // Opcion r - muestra frecuencia relativa
bool wflag = false; // Opcion w - muestra las palabras mas frecuentes especificando la cantidad que se va a mostrar

// Declaracion de estructura de nodo para lista enlazada de palabras y su frecuencia
typedef struct node
{
    char word[MAX_BUFFER_SIZE];
    int frequency;
    struct node *next;
} node_t;

// Imprime ayuda de uso de programa
void print_help()
{
    printf("\nwordcount imprime las palabras mas frecuentes en un archivo de texto.\n");
    printf("uso:\n");
    printf("./wordcount [-w <num palabras>] [-r] [<nombre archivo>]\n");
    printf("./wordcount -h\n");
    printf("Opciones:\n");
    printf("-h\t\t\tAyuda, muestra este mensaje\n");
    printf("-w <num palabras>\tEspecifica el numero de palabras a mostrar [default: 20]\n");
    printf("-r\t\t\tMuestra frecuencia relativa\n");
}

// Recibe: lista enlazada, cadena de caracteres a buscar
// Devuelve: un puntero al nodo en caso de que exista; caso contrario, devuelve NULL
node_t *find(node_t *list, char *word)
{
    node_t *nodo_actual = list;
    while (nodo_actual != NULL)
    {
        if (strcmp(nodo_actual->word, word) == 0)
        {
            return nodo_actual;
        }
        nodo_actual = nodo_actual->next;
    }
    return NULL;
}

// Recibe: cadena de caracteres para nuevo nodo
// Devuelve: un puntero al nuevo nodo
node_t *create(char *word)
{
    node_t *nuevo_nodo = (node_t *)malloc(sizeof(node_t));
    if (nuevo_nodo != NULL)
    {
        strcpy(nuevo_nodo->word, word);
        nuevo_nodo->frequency = 1;
        nuevo_nodo->next = NULL;
    }
    return nuevo_nodo;
}

// Recibe: un puntero al puntero del inicio de la lista, palabra para verificar que exista
// Devuelve: nada, modifica la lista
void push(node_t **list, char *word)
{
    node_t *nodo_actual = find(*list, word);
    if (nodo_actual != NULL)
    {
        nodo_actual->frequency++;
    }
    else
    {
        node_t *nuevo_nodo = create(word);
        if (nuevo_nodo != NULL)
        {
            nuevo_nodo->next = *list;
            *list = nuevo_nodo;
        }
    }
}

// Recibe: un puntero al nodo inicial
// Devuelve: tamano de la lista
int count(node_t *head)
{
    int count = 0;
    node_t *current = head;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count;
}

// Algoritmo de ordenamiento -> mayor a menor frecuencia
void bubble_sort(node_t **list)
{
    node_t *current, *prev, *next;
    int swapped;

    do
    {
        swapped = 0;
        current = *list;
        prev = NULL;

        while (current != NULL && current->next != NULL)
        {
            next = current->next;

            if (current->frequency < next->frequency)
            {
                if (prev != NULL)
                {
                    prev->next = next;
                }
                else
                {
                    *list = next;
                }

                current->next = next->next;
                next->next = current;
                prev = next;
                swapped = 1;
            }
            else
            {
                prev = current;
                current = current->next;
            }
        }
    } while (swapped);
}

void print(node_t *head, int size, int cant_palabras, int cant_mostrar)
{
    int i = 0, show = 0;
    if (cant_mostrar > size)
        fprintf(stderr, "ERROR: Cantidad de palabras mayor a la permitida -> %d \n", size);
    else
        show = cant_mostrar;

    if (rflag)
    {
        node_t *ptr;
        ptr = head;
        while (show > i && ptr != NULL)
        {
            printf("%-30s\t%-20.2f\n", ptr->word, (double)(ptr->frequency) / cant_palabras);
            ptr = ptr->next;
            i++;
        }
    }
    else
    {
        node_t *ptr;
        ptr = head;
        while (show > i && ptr != NULL)
        {
            printf("%-30s\t%-20hd\n", ptr->word, ptr->frequency);
            ptr = ptr->next;
            i++;
        }
    }
}

int main(int argc, char *argv[])
{
    int opt, i;
    int default_show = 20;
    int total_words = 0;
    char *file_name;
    FILE *file;

    node_t *nodo_actual;
    node_t *header;

    // Si la cantidad de argumentos es menor a 2, se muestra mensaje de ayuda
    if (argc < 2)
    {
        fprintf(stderr, "ERROR: Argumentos insuficientes\n");
        print_help();
        return 1;
    }

    while ((opt = getopt(argc, argv, "hrw")) != -1)
    {
        switch (opt)
        {
        case 'h':
            print_help();
            return 1;
            break;
        case 'w':
            wflag = true;
            break;
        case 'r':
            rflag = true;
            break;
        case '?':
        default:
            fprintf(stderr, "uso: \t%s [-w <num palabras>] [-r] [<nombre archivo>]\n", argv[0]);
            fprintf(stderr, "\t%s -h\n", argv[0]);
            return 1;
        }
    }

    // Asigna la cantidad de palabras a mostrar, de no encontrar,
    // se queda con el valor default
    for (i = optind; i < argc; i++)
    {
        if ((strtol(argv[i], NULL, 10) != 0))
            default_show = strtol(argv[i], NULL, 10);
        file_name = argv[i];
    }

    // Si no se ingreso nombre de archivo, muestra error de que no se ha indicado archivo
    if (file_name == NULL)
    {
        fprintf(stderr, "ERROR: No se ha indicado archivo\n");
        fprintf(stderr, "uso: \t%s [-w <num palabras>] [-r] [<nombre archivo>]\n", argv[0]);
        fprintf(stderr, "\t%s -h\n", argv[0]);
        return 1;
    }

    file = fopen(file_name, "r");

    // En caso de no haberse podido abrir el archivo
    if (file == NULL)
    {
        fprintf(stderr, "ERROR: No se pudo abrir archivo %s - %s\n", file_name, strerror(errno));
        return 1;
    }

    char delimitador[] = ".,:;\n";                          // Caracteres ignorados
    char *palabra = malloc(MAX_BUFFER_SIZE * sizeof(char)); // Contenedor para palabras del archivo

    // Cuenta el total de palabras del archivo
    while (!feof(file))
    {
        fscanf(file, "%s", palabra);
        total_words++;
    }

    rewind(file);

    // Comienza a agregar nodos de ser palabra nueva, caso contrario, suma +1 a la frecuencia
    while (fscanf(file, "%s", palabra) == 1)
    {
        char *token = strtok(palabra, delimitador);
        while (token != NULL)
        {
            push(&header, token);
            token = strtok(NULL, delimitador);
        }
    }

    free(palabra);

    fclose(file);

    bubble_sort(&header);
    print(header, count(header), total_words, default_show);
    return 0;
}