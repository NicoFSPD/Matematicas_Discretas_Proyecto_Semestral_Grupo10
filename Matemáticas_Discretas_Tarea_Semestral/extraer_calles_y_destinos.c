#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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

int obtener_Calles(FILE *archivo, Calle calles[], int *cantidad_calles) {
    char linea_del_archivo[256];
    int calles_leidas = 0;
    int numero_de_calles = 0;

    if (fgets(linea_del_archivo, sizeof(linea_del_archivo), archivo) == NULL) {
        printf("El archivo esta vacio.\n");
        return 0;
    }

    numero_de_calles = (int)strtol(linea_del_archivo, NULL, 10);
    if (numero_de_calles < 1 || numero_de_calles > 50) {
        printf("El numero de calles debe estar entre 1 y 50\n");
        return 0;
    }

    for (calles_leidas = 0; calles_leidas < numero_de_calles; calles_leidas++) {
        if (fgets(linea_del_archivo, sizeof(linea_del_archivo), archivo) == NULL) {
            printf("Faltan calles. Se esperaban %d y se leyeron %d.\n", numero_de_calles, calles_leidas);
            return 0;
        }

        {
            double x_inicio;
            double y_inicio;
            double x_final;
            double y_final;
            if (sscanf(
                    linea_del_archivo,
                    "%99s %lf %lf %lf %lf %c",
                    calles[calles_leidas].nombre_calle,
                    &x_inicio,
                    &y_inicio,
                    &x_final,
                    &y_final,
                    &calles[calles_leidas].eje_calle
                ) != 6) {
                printf("Formato invalido en calle %d. Debe ser: Nombre_Calle Coordenada_X_Inicio_Calle Coordenada_Y_Inicio_Calle Coordenada_X_Final_Calle Coordenada_Y_Final_Calle Eje_Calle\n", calles_leidas + 1);
                return 0;
            }

            calles[calles_leidas].coordenada_x_inicio = x_inicio;
            calles[calles_leidas].coordenada_y_inicio = y_inicio;
            calles[calles_leidas].coordenada_x_final = x_final;
            calles[calles_leidas].coordenada_y_final = y_final;
        }

        calles[calles_leidas].eje_calle = (char)toupper((unsigned char)calles[calles_leidas].eje_calle);
        if (calles[calles_leidas].eje_calle != 'X' && calles[calles_leidas].eje_calle != 'Y') {
            printf("Eje invalido en calle %d. Debe ser X o Y.\n", calles_leidas + 1);
            return 0;
        }
    }

    *cantidad_calles = calles_leidas;
    return 1;
}

int obtener_Destinos(FILE *archivo, Destino destinos[], int *cantidad_destinos) {
    char linea_del_archivo[256];
    int destinos_leidos = 0;
    int numero_de_destinos = 0;

    if (fgets(linea_del_archivo, sizeof(linea_del_archivo), archivo) == NULL) {
        printf("Falta la linea con el numero de destinos.\n");
        return 0;
    }

    {
        char nombre_tmp[100];
        double xi, yi, xf, yf;
        char eje_tmp;
        if (sscanf(linea_del_archivo, "%99s %lf %lf %lf %lf %c", nombre_tmp, &xi, &yi, &xf, &yf, &eje_tmp) == 6) {
            printf("Hay mas calles de las que deberian.\n");
            return 0;
        }
    }

    {
        char *fin_numero;
        numero_de_destinos = (int)strtol(linea_del_archivo, &fin_numero, 10);
        while (*fin_numero != '\0' && isspace((unsigned char)*fin_numero)) {
            fin_numero++;
        }
        if (fin_numero == linea_del_archivo || *fin_numero != '\0') {
            printf("Formato invalido para numero de destinos.\n");
            return 0;
        }
    }

    if (numero_de_destinos < 0 || numero_de_destinos > 200) {
        printf("El numero de destinos debe estar entre 0 y 200\n");
        return 0;
    }

    for (destinos_leidos = 0; destinos_leidos < numero_de_destinos; destinos_leidos++) {
        if (fgets(linea_del_archivo, sizeof(linea_del_archivo), archivo) == NULL) {
            printf("Faltan destinos. Se esperaban %d y se leyeron %d.\n", numero_de_destinos, destinos_leidos);
            return 0;
        }

        {
            double posicion_tmp;
            if (sscanf(linea_del_archivo, "%99s %99s %lf", destinos[destinos_leidos].nombre_destino, destinos[destinos_leidos].calle, &posicion_tmp) != 3) {
                printf("Formato invalido en destino %d. Debe ser: nombre_destino calle posicion\n", destinos_leidos + 1);
                return 0;
            }
            destinos[destinos_leidos].posicion = posicion_tmp;
        }
    }

    *cantidad_destinos = destinos_leidos;
    return 1;
}
