#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char nombre_calle[100];
    double coordenada_x_inicio;
    double coordenada_y_inicio;
    double coordenada_x_final;
    double coordenada_y_final;
    char eje_calle;
} Calle;

typedef struct {
    char nombre_destino[100];
    char calle[100];
    double posicion;
} Destino;

int obtener_Calles(FILE *archivo, Calle calles[], int *cantidad_calles);
int obtener_Destinos(FILE *archivo, Destino destinos[], int *cantidad_destinos);
int ejecutar_mapa(void);
int ejecutar_obtener_nodos(void);
int ejecutar_obtener_ruta(void);

static int coordenadas_validas(const Calle *calle) {
    if (calle->coordenada_x_inicio < 0 || calle->coordenada_x_inicio > 2000) {
        return 0;
    }
    if (calle->coordenada_y_inicio < 0 || calle->coordenada_y_inicio > 2000) {
        return 0;
    }
    if (calle->coordenada_x_final < 0 || calle->coordenada_x_final > 2000) {
        return 0;
    }
    if (calle->coordenada_y_final < 0 || calle->coordenada_y_final > 2000) {
        return 0;
    }

    return 1;
}

static int procesar_archivo_txt(void) {
    FILE *c = NULL;
    char nombre_archivo[256];
    Calle calles[50];
    Destino destinos[200];
    int calles_leidas = 0;
    int destinos_leidos = 0;

    while (c == NULL) {
        printf("Ingrese el nombre del archivo a leer: ");
        if (fgets(nombre_archivo, sizeof(nombre_archivo), stdin) == NULL) {
            printf("Error leyendo el nombre del archivo.\n");
            return 0;
        }

        nombre_archivo[strcspn(nombre_archivo, "\n")] = '\0';
        if (nombre_archivo[0] == '\0') {
            printf("Debe ingresar un nombre de archivo valido.\n");
            continue;
        }

        c = fopen(nombre_archivo, "r");
        if (c == NULL) {
            printf("No se pudo abrir el archivo '%s'. Intente nuevamente.\n", nombre_archivo);
        }
    }

    if (!obtener_Calles(c, calles, &calles_leidas)) {
        fclose(c);
        return 0;
    }

    if (!obtener_Destinos(c, destinos, &destinos_leidos)) {
        fclose(c);
        return 0;
    }

    printf("\nCalles almacenadas correctamente:\n");
    for (int i = 0; i < calles_leidas; i++) {
        if (!coordenadas_validas(&calles[i])) {
            printf("Error: la calle '%s' tiene coordenadas fuera del rango permitido (0 a 2000).\n",
                   calles[i].nombre_calle);
            fclose(c);
            return 0;
        }

        printf("%s %.2f %.2f %.2f %.2f %c\n",
               calles[i].nombre_calle,
               calles[i].coordenada_x_inicio,
               calles[i].coordenada_y_inicio,
               calles[i].coordenada_x_final,
               calles[i].coordenada_y_final,
               calles[i].eje_calle);
    }

    printf("\nDestinos almacenados correctamente:\n");
    for (int i = 0; i < destinos_leidos; i++) {
        printf("%s %s %.2f\n",
               destinos[i].nombre_destino,
               destinos[i].calle,
               destinos[i].posicion);
    }

    FILE *salida_calles = fopen("1calles_guardadas.txt", "w");
    if (salida_calles == NULL) {
        printf("No se pudo abrir 1calles_guardadas.txt para escritura.\n");
        fclose(c);
        return 0;
    }
    for (int i = 0; i < calles_leidas; i++) {
        fprintf(salida_calles,
                "%s %.2f %.2f %.2f %.2f %c\n",
                calles[i].nombre_calle,
                calles[i].coordenada_x_inicio,
                calles[i].coordenada_y_inicio,
                calles[i].coordenada_x_final,
                calles[i].coordenada_y_final,
                calles[i].eje_calle);
    }
    fclose(salida_calles);

    FILE *salida_destinos = fopen("2destinos_guardados.txt", "w");
    if (salida_destinos == NULL) {
        printf("No se pudo abrir 2destinos_guardados.txt para escritura.\n");
        fclose(c);
        return 0;
    }
    for (int i = 0; i < destinos_leidos; i++) {
        fprintf(salida_destinos,
                "%s %s %.2f\n",
                destinos[i].nombre_destino,
                destinos[i].calle,
                destinos[i].posicion);
    }
    fclose(salida_destinos);
    fclose(c);

    if (ejecutar_mapa() != 0) {
        printf("Error ejecutando el modulo mapa.\n");
        return 0;
    }
    if (ejecutar_obtener_nodos() != 0) {
        printf("Error ejecutando el modulo obtener_nodos.\n");
        return 0;
    }
    if (ejecutar_obtener_ruta() != 0) {
        printf("Error ejecutando el modulo obtener_ruta.\n");
        return 0;
    }

    return 1;
}

int main() {
    int continuar = 1;
    char nombre_archivo[256];

    while (continuar) {
        if (!procesar_archivo_txt()) {
            printf("\nOcurrio un error. Vuelva a ingresar el archivo .txt.\n");
            continue;
        }

        printf("\n Desea ingresar otro archivo .txt? (s/n): ");
        if (fgets(nombre_archivo, sizeof(nombre_archivo), stdin) == NULL) {
            return 1;
        }

        if (nombre_archivo[0] == 's' || nombre_archivo[0] == 'S') {
            continuar = 1;
        } else {
            continuar = 0;
        }
    }

    return 0;
}

