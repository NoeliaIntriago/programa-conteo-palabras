#include <stdio.h>   // Libreria para manipulacion de archivos
#include <getopt.h>  // Libreria para procesar los argumentos pasados en la terminal
#include <stdbool.h> // Libreria para uso de booleanos
#include <errno.h>   // Libreria para manejo de errores
#include <string.h>  // Libreria para manipulacion de cadenas de caracteres
#include <ctype.h>   // Libreria para estados de una variable
#include <stdlib.h>  // Libreria para conversiones y gestion de memoria
#define MAX_BUFFER_SIZE 512

bool rflag = false; // Opcion r - muestra frecuencia relativa

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

// Recibe: un puntero al nodo inicial
// Devuelve: nada, libera la memoria de la lista enlazada
void free_list(node_t *head)
{
    while (head)
    {
        node_t *temp = head;
        head = head->next;
        free(temp);
    }
}

// Recibe: lista enlazada, cadena de caracteres a buscar
// Devuelve: un puntero al nodo en caso de que exista; caso contrario, devuelve NULL
node_t *find(node_t *list, char *word)
{
    while (list)
    {
        if (strcmp(list->word, word) == 0)
        {
            return list;
        }
        list = list->next;
    }
    return NULL;
}

// Recibe: cadena de caracteres para nuevo nodo
// Devuelve: un puntero al nuevo nodo
node_t *create(char *word)
{
    node_t *nuevo_nodo = malloc(sizeof(node_t));
    if (nuevo_nodo == NULL)
    {
        fprintf(stderr, "ERROR: No se pudo asignar memoria para el nuevo nodo\n");
        exit(EXIT_FAILURE);
    }

    strcpy(nuevo_nodo->word, word);
    nuevo_nodo->frequency = 1;
    nuevo_nodo->next = NULL;
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
        nuevo_nodo->next = *list;
        *list = nuevo_nodo;
    }
}

// Recibe: un puntero al nodo inicial
// Devuelve: tamano de la lista
int count(node_t *head)
{
    int count = 0;
    while (head)
    {
        count++;
        head = head->next;
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
                if (prev)
                    prev->next = next;
                else
                    *list = next;

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

// Recibe: un puntero al nodo inicial, cantidad total de palabras, cantidad maxima a mostrar
// Devuelve: nada, imprime las palabras y sus frecuencias
void print(node_t *head, int total_words, int max)
{
    int i = 0;
    node_t *ptr = head;

    if (max > total_words)
    {
        fprintf(stderr, "WARN: Cantidad de palabras a mostrar mayor a la cantidad total de palabras -> %d \n", total_words);
        max = total_words;
    }

    while (ptr && i < max)
    {
        if (rflag)
            printf("%-30s\t%-20.2f\n", ptr->word, (double)(ptr->frequency) / total_words);
        else
            printf("%-30s\t%-20hd\n", ptr->word, ptr->frequency);
        ptr = ptr->next;
        i++;
    }
}

// Recibe: cadena de caracteres
// Devuelve: cadena de caracteres en minusculas
void process_file(FILE *file, node_t **header, int *total_words)
{
    char palabra[MAX_BUFFER_SIZE];
    int index = 0, c;

    while ((c = fgetc(file)) != EOF)
    {
        if (isalpha(c))
        {
            if (index < MAX_BUFFER_SIZE - 1)
            {
                palabra[index++] = tolower(c);
            }
        }
        else
        {
            if (index > 0)
            {
                palabra[index] = '\0'; // Termina la cadena
                push(header, palabra); // Agrega la palabra a la lista enlazada
                (*total_words)++;
                index = 0; // Reinicia el índice para la siguiente palabra
            }
        }
    }

    if (index > 0)
    {
        palabra[index] = '\0'; // Termina la cadena
        push(header, palabra); // Agrega la última palabra si existe
        (*total_words)++;
    }
}

int main(int argc, char *argv[])
{
    int opt, default_show = 20, total_words = 0;
    char *file_name;
    FILE *file;
    node_t *header = NULL;

    // Si la cantidad de argumentos es menor a 2, se muestra mensaje de ayuda
    if (argc < 2)
    {
        fprintf(stderr, "ERROR: Argumentos insuficientes\n");
        print_help();
        return 1;
    }

    while ((opt = getopt(argc, argv, "hrw:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            print_help();
            return 0;
        case 'w':
            default_show = atoi(optarg);
            if (default_show <= 0)
                default_show = 20; // Si el valor es invalido, se asigna el valor por defecto
            break;
        case 'r':
            rflag = true;
            break;
        default:
            fprintf(stderr, "uso: \t%s [-w <num palabras>] [-r] [<nombre archivo>]\n", argv[0]);
            fprintf(stderr, "\t%s -h\n", argv[0]);
            return 1;
        }
    }

    // Si no se especifica un archivo, se muestra mensaje de ayuda
    if (optind < argc)
    {
        file_name = argv[optind];
    }
    else
    {
        fprintf(stderr, "ERROR: No se indicó archivo.\n");
        print_help();
        return 1;
    }

    file = fopen(file_name, "r");

    // En caso de no haberse podido abrir el archivo
    if (file == NULL)
    {
        fprintf(stderr, "ERROR: No se pudo abrir archivo %s - %s\n", file_name, strerror(errno));
        return 1;
    }

    // Procesa el archivo y cuenta las palabras
    process_file(file, &header, &total_words);
    fclose(file);

    bubble_sort(&header);
    print(header, count(header), default_show);

    free_list(header); // Libera la memoria de la lista enlazada
    return 0;
}