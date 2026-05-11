#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
	char nombre_calle[100];
	int coordenada_x_inicio;
	int coordenada_y_inicio;
	int coordenada_x_final;
	int coordenada_y_final;
	char eje_calle;
} Calle;

typedef struct {
	char nombre_destino[100];
	char calle[100];
	int posicion;
} Destino;

typedef struct {
	int x;
	int y;
	int valido;
	int en_rango;
} Coordenada;

static int minimo(int a, int b) {
	if (a < b) {
		return a;
	}
	return b;
}

static int maximo(int a, int b) {
	if (a > b) {
		return a;
	}
	return b;
}


//funcion para encontrar el indice correspondiente a la calle por su nombre, o -1 si no se encuentra
static int buscar_indice_calle(const Calle calles[], int cantidad_calles, const char *nombre) {
	for (int i = 0; i < cantidad_calles; i++) {
		if (strcmp(calles[i].nombre_calle, nombre) == 0) {
			return i;
		}
	}
	return -1;
}
//Esta funcion recibe la calle con sus puntos iniciales y finales. El posicion es el destino en esa calle.
//IMPORTANTE: Como se obtiene un eje de calle, para las horizontales y verticales, se hace el mismo procedimiento.
//Tuve que implementar interpolación lineal pq si no las diagonales las tomaba como vertical u horizontal solamente modificando el valor puesto en la entrada.
static Coordenada convertir_posicion_a_coordenada(Calle calle, int posicion) {

	Coordenada resultado;
	long long numerador;
	long long denominador;
	resultado.x = 0;
	resultado.y = 0;
	resultado.valido = 0;
	resultado.en_rango = 0;

    //Caso horizontal
	if (calle.eje_calle == 'X') {

		if (calle.coordenada_x_inicio == calle.coordenada_x_final) {
			return resultado;
		}
		resultado.x = posicion;
		numerador = (long long)(posicion - calle.coordenada_x_inicio) *
			(long long)(calle.coordenada_y_final - calle.coordenada_y_inicio);
		denominador = (long long)(calle.coordenada_x_final - calle.coordenada_x_inicio);
		resultado.y = calle.coordenada_y_inicio + (int)(numerador / denominador);

		//Verificación de que la posición dada esté dentro del rango de la calle. Se compara con los extremos de X.
		resultado.en_rango = (posicion >= minimo(calle.coordenada_x_inicio, calle.coordenada_x_final) &&
							  posicion <= maximo(calle.coordenada_x_inicio, calle.coordenada_x_final));
		resultado.valido = 1;
		return resultado;
	}

	//Caso vertical
	if (calle.eje_calle == 'Y') {
		if (calle.coordenada_y_inicio == calle.coordenada_y_final) {
			return resultado;
		}
		resultado.y = posicion;

        //interpolacion lineal
		numerador = (long long)(posicion - calle.coordenada_y_inicio) *
			(long long)(calle.coordenada_x_final - calle.coordenada_x_inicio);
		denominador = (long long)(calle.coordenada_y_final - calle.coordenada_y_inicio);
		resultado.x = calle.coordenada_x_inicio + (int)(numerador / denominador);

		/* Validacion equivalente al caso anterior, pero en Y. */
		resultado.en_rango = (posicion >= minimo(calle.coordenada_y_inicio, calle.coordenada_y_final) &&
							  posicion <= maximo(calle.coordenada_y_inicio, calle.coordenada_y_final));
		resultado.valido = 1;
		return resultado;
	}

	return resultado;
}

static int leer_calles(const char *archivo, Calle calles[], int *cantidad_calles) {
	FILE *entrada = fopen(archivo, "r");
	char linea[256];
	int contador = 0;

	if (entrada == NULL) {
		printf("No se pudo abrir '%s'.\n", archivo);
		return 0;
	}

	while (fgets(linea, sizeof(linea), entrada) != NULL && contador < 50) {
		if (sscanf(linea,
				   "%99s %d %d %d %d %c",
				   calles[contador].nombre_calle,
				   &calles[contador].coordenada_x_inicio,
				   &calles[contador].coordenada_y_inicio,
				   &calles[contador].coordenada_x_final,
				   &calles[contador].coordenada_y_final,
				   &calles[contador].eje_calle) != 6) {
			printf("Formato invalido en calle %d.\n", contador + 1);
			fclose(entrada);
			return 0;
		}

		calles[contador].eje_calle = (char)toupper((unsigned char)calles[contador].eje_calle);
		contador++;
	}

	fclose(entrada);
	*cantidad_calles = contador;
	return 1;
}

static int leer_destinos(const char *archivo, Destino destinos[], int *cantidad_destinos) {
	FILE *entrada = fopen(archivo, "r");
	char linea[256];
	int contador = 0;

	if (entrada == NULL) {
		printf("No se pudo abrir '%s'.\n", archivo);
		return 0;
	}

	while (fgets(linea, sizeof(linea), entrada) != NULL && contador < 200) {
		if (sscanf(linea, "%99s %99s %d", destinos[contador].nombre_destino, destinos[contador].calle, &destinos[contador].posicion) != 3) {
			printf("Formato invalido en destino %d.\n", contador + 1);
			fclose(entrada);
			return 0;
		}
		contador++;
	}

	fclose(entrada);
	*cantidad_destinos = contador;
	return 1;
}

int ejecutar_mapa(void) {
	Calle calles[50];
	Destino destinos[200];
	int cantidad_calles = 0;
	int cantidad_destinos = 0;
	FILE *salida = NULL;

	if (!leer_calles("1calles_guardadas.txt", calles, &cantidad_calles)) {
		return 1;
	}

	if (!leer_destinos("2destinos_guardados.txt", destinos, &cantidad_destinos)) {
		return 1;
	}

	// Guarda el resultado en el archivo de coordenadas.
	salida = fopen("3coordenadas_guardadas.txt", "w");
	if (salida == NULL) {
		printf("No se pudo abrir '3coordenadas_guardadas.txt' para escritura.\n");
		return 1;
	}

	// Para cada destino: busca su calle, calcula coordenada y valida rango. 
    //Aquí están todas las validaciones correspondientes
	for (int i = 0; i < cantidad_destinos; i++) {
		int indice_calle = buscar_indice_calle(calles, cantidad_calles, destinos[i].calle);
		Coordenada coordenada;

		if (indice_calle < 0) {
			printf("%s: calle no encontrada o eje invalido (%s)\n", destinos[i].nombre_destino, destinos[i].calle);
			fprintf(salida, "%s: calle no encontrada o eje invalido (%s)\n", destinos[i].nombre_destino, destinos[i].calle);
			continue;
		}

		coordenada = convertir_posicion_a_coordenada(calles[indice_calle], destinos[i].posicion);

		if (!coordenada.valido) {
			printf("%s: calle no encontrada o eje invalido (%s)\n", destinos[i].nombre_destino, destinos[i].calle);
			fprintf(salida, "%s: calle no encontrada o eje invalido (%s)\n", destinos[i].nombre_destino, destinos[i].calle);
			continue;
		}

		if (!coordenada.en_rango) {
			printf("%s: coordenada (%d, %d) fuera del rango de la calle %s\n",
				   destinos[i].nombre_destino,
				   coordenada.x,
				   coordenada.y,
				   destinos[i].calle);
			fprintf(salida,
					"%s: coordenada (%d, %d) fuera del rango de la calle %s\n",
					destinos[i].nombre_destino,
					coordenada.x,
					coordenada.y,
					destinos[i].calle);
			continue;
		}

		printf("%s: coordenada (%d, %d)\n", destinos[i].nombre_destino, coordenada.x, coordenada.y);
		fprintf(salida, "%s %d %d\n", destinos[i].nombre_destino, coordenada.x, coordenada.y);
	}

	fclose(salida);

	return 0;
}
