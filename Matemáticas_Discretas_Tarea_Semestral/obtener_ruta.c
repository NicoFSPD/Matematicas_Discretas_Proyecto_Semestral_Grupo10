#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Limites maximos de datos que manejamos en memoria.
#define MAX_CALLES 100
#define MAX_NODOS 1000
#define MAX_DESTINOS 200
// Valor grande para representar "sin conexion" en la matriz.
#define INF 1000000000

typedef struct {
	char nombre[100];
	double x1;
	double y1;
	double x2;
	double y2;
	char eje;
} Calle;

// Nodo que se utilizará en el algoritmo posterior
typedef struct {
	char etiqueta_a[100];
	char etiqueta_b[100];
	double x;
	double y;
} Nodo;

typedef struct {
	char nombre[100];
	double x;
	double y;
} Destino;

static double minimo_double(double a, double b) {
	if (a < b) {
		return a;
	}
	return b;
}

static double maximo_double(double a, double b) {
	if (a > b) {
		return a;
	}
	return b;
}

static int casi_igual(double a, double b) {
	const double eps = 1e-9;
	return fabs(a - b) < eps;
}

// Algoritmo para ver si un nodo cae en una calle
static int punto_en_segmento(double px, double py, double ax, double ay, double bx, double by) {
	double abx = bx - ax;
	double aby = by - ay;
	double apx = px - ax;
	double apy = py - ay;
	double cruz = abx * apy - aby * apx;

	if (!casi_igual(cruz, 0.0)) {
		return 0;
	}

	if (px < minimo_double(ax, bx) - 1e-9 || px > maximo_double(ax, bx) + 1e-9) {
		return 0;
	}

	if (py < minimo_double(ay, by) - 1e-9 || py > maximo_double(ay, by) + 1e-9) {
		return 0;
	}

	return 1;
}

// Lee calles desde calles_guardadas.txt.
static int leer_calles(const char *archivo, Calle calles[], int *cantidad_calles) {
	FILE *entrada = fopen(archivo, "r");
	char linea[256];
	int contador = 0;

	if (entrada == NULL) {
		printf("No se pudo abrir '%s'.\n", archivo);
		return 0;
	}

	while (fgets(linea, sizeof(linea), entrada) != NULL && contador < MAX_CALLES) {
		if (sscanf(linea,
				   "%99s %lf %lf %lf %lf %c",
				   calles[contador].nombre,
				   &calles[contador].x1,
				   &calles[contador].y1,
				   &calles[contador].x2,
				   &calles[contador].y2,
				   &calles[contador].eje) != 6) {
			printf("Formato invalido en 1calles_guardadas.txt, linea %d.\n", contador + 1);
			fclose(entrada);
			return 0;
		}
		contador++;
	}

	fclose(entrada);
	*cantidad_calles = contador;
	return 1;
}

// Verifica si ya existe un nodo con esas coordenadas.
// Evita duplicar nodos en memoria.
static int existe_nodo(Nodo nodos[], int cantidad_nodos, double x, double y) {
	for (int i = 0; i < cantidad_nodos; i++) {
		if (casi_igual(nodos[i].x, x) && casi_igual(nodos[i].y, y)) {
			return 1;
		}
	}
	return 0;
}

// Lee nodos desde 4nodos.txt.
// Si hay coordenadas repetidas, se conserva solo una.
static int leer_nodos(const char *archivo, Nodo nodos[], int *cantidad_nodos) {
	FILE *entrada = fopen(archivo, "r");
	char linea[256];
	int contador = 0;

	if (entrada == NULL) {
		printf("No se pudo abrir '%s'.\n", archivo);
		return 0;
	}

	while (fgets(linea, sizeof(linea), entrada) != NULL && contador < MAX_NODOS) {
		Nodo n;
		if (sscanf(linea, "%99s %99s %lf %lf", n.etiqueta_a, n.etiqueta_b, &n.x, &n.y) == 4) {
			if (!existe_nodo(nodos, contador, n.x, n.y)) {
				nodos[contador] = n;
				contador++;
			}
		}
	}

	fclose(entrada);
	*cantidad_nodos = contador;
	return 1;
}

// Lee destinos desde 3coordenadas_guardadas.txt.
static int leer_destinos(const char *archivo, Destino destinos[], int *cantidad_destinos) {
	FILE *entrada = fopen(archivo, "r");
	char linea[256];
	int contador = 0;

	if (entrada == NULL) {
		printf("No se pudo abrir '%s'.\n", archivo);
		return 0;
	}

	while (fgets(linea, sizeof(linea), entrada) != NULL && contador < MAX_DESTINOS) {
		if (sscanf(linea, "%99s %lf %lf", destinos[contador].nombre, &destinos[contador].x, &destinos[contador].y) == 3) {
			contador++;
		}
	}

	fclose(entrada);
	*cantidad_destinos = contador;
	return 1;
}

// Busca el indice de un nodo segun coordenadas exactas.
// Retorna -1 si no existe.
static int indice_nodo_por_coord(Nodo nodos[], int cantidad_nodos, double x, double y) {
	for (int i = 0; i < cantidad_nodos; i++) {
		if (casi_igual(nodos[i].x, x) && casi_igual(nodos[i].y, y)) {
			return i;
		}
	}
	return -1;
}

// Ordena nodos que estan en una misma calle de acuerdo a su distancia
// al punto inicial de esa calle. Asi podemos conectar nodos consecutivos.
static void ordenar_por_distancia_inicio(const Calle *c, Nodo nodos[], int indices[], int cantidad) {
	for (int i = 1; i < cantidad; i++) {
		int actual = indices[i];
		double d_actual;
		int j = i - 1;

		d_actual = (nodos[actual].x - c->x1) * (nodos[actual].x - c->x1) +
				   (nodos[actual].y - c->y1) * (nodos[actual].y - c->y1);

		while (j >= 0) {
			int previo = indices[j];
			double d_previo = (nodos[previo].x - c->x1) * (nodos[previo].x - c->x1) +
							 (nodos[previo].y - c->y1) * (nodos[previo].y - c->y1);
			if (d_previo <= d_actual) {
				break;
			}
			indices[j + 1] = indices[j];
			j--;
		}

		indices[j + 1] = actual;
	}
}

// Iniciar la matriz de adjacencia, se usó un algoritmo ya creado para otra tarea similar:
// ady[i][j] = 0 si i==j
// ady[i][j] = INF si no hay conexion conocida
// calle_arista[i][j] = indice de calle usada entre i y j (-1 si no existe)
static void inicializar_matriz(int ady[MAX_NODOS][MAX_NODOS], int calle_arista[MAX_NODOS][MAX_NODOS], int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i == j) {
				ady[i][j] = 0;
			} else {
				ady[i][j] = INF;
			}
			calle_arista[i][j] = -1;
		}
	}
}

// Construye la matriz de adyacencia a partir de calles + nodos.
// Idea:
// 1) Para cada calle, buscar todos los nodos que caen sobre ella.
// 2) Ordenarlos en el sentido de la calle.
// 3) Conectar cada par consecutivo con valor 1.
// Esto genera un grafo no dirigido de nodos conectados por calles.
static void construir_adyacencia(const Calle calles[],
								 int cantidad_calles,
								 Nodo nodos[],
								 int cantidad_nodos,
								 int ady[MAX_NODOS][MAX_NODOS],
								 int calle_arista[MAX_NODOS][MAX_NODOS]) {
	int indices[MAX_NODOS];

	inicializar_matriz(ady, calle_arista, cantidad_nodos);

	for (int c = 0; c < cantidad_calles; c++) {
		int cantidad = 0;

		for (int i = 0; i < cantidad_nodos; i++) {
			if (punto_en_segmento(nodos[i].x,
								  nodos[i].y,
								  calles[c].x1,
								  calles[c].y1,
								  calles[c].x2,
								  calles[c].y2)) {
				indices[cantidad] = i;
				cantidad++;
			}
		}

		if (cantidad < 2) {
			continue;
		}

		ordenar_por_distancia_inicio(&calles[c], nodos, indices, cantidad);

		for (int i = 0; i < cantidad - 1; i++) {
			int u = indices[i];
			int v = indices[i + 1];
			ady[u][v] = 1;
			ady[v][u] = 1;
			calle_arista[u][v] = c;
			calle_arista[v][u] = c;
		}
	}
}

// Implementacion de Dijkstra reutilizando la idea del otro proyecto:
// La unica diferencia que si permite volver al nodo origen, pero solo si el destino es el mismo nodo origen (camino de largo 0).
static int dijkstra_reutilizado(int n,
								int ady[MAX_NODOS][MAX_NODOS],
								int origen,
								int destino,
								int camino[],
								int *largo_camino) {
	int dist[MAX_NODOS];
	int prev[MAX_NODOS];
	int visitado[MAX_NODOS];
	int temporal[MAX_NODOS];
	int largo = 0;

	for (int i = 0; i < n; i++) {
		dist[i] = INF;
		prev[i] = -1;
		visitado[i] = 0;
	}

	dist[origen] = 0;

	for (int iter = 0; iter < n; iter++) {
		int u = -1;
		int mejor = INF;

		for (int i = 0; i < n; i++) {
			if (!visitado[i] && dist[i] < mejor) {
				mejor = dist[i];
				u = i;
			}
		}

		if (u < 0) {
			break;
		}

		if (u == destino) {
			break;
		}

		visitado[u] = 1;

		for (int v = 0; v < n; v++) {
			if (ady[u][v] >= INF || ady[u][v] == 0 || visitado[v]) {
				continue;
			}
			if (dist[u] + ady[u][v] < dist[v]) {
				dist[v] = dist[u] + ady[u][v];
				prev[v] = u;
			}
		}
	}

	if (origen == destino) {
		camino[0] = origen;
		*largo_camino = 1;
		return 1;
	}

	if (prev[destino] == -1) {
		*largo_camino = 0;
		return 0;
	}

	{
		int actual = destino;
		while (actual != -1) {
			temporal[largo] = actual;
			largo++;
			actual = prev[actual];
		}
	}

	for (int i = 0; i < largo; i++) {
		camino[i] = temporal[largo - 1 - i];
	}

	*largo_camino = largo;
	return 1;
}

// Guarda los nodos
static int guardar_nodos_numerados(const char *archivo, Nodo nodos[], int cantidad_nodos) {
	FILE *salida = fopen(archivo, "w");

	if (salida == NULL) {
		return 0;
	}

	for (int i = 0; i < cantidad_nodos; i++) {
		fprintf(salida,
				"N%d %.2f %.2f %s %s\n",
				i + 1,
				nodos[i].x,
				nodos[i].y,
				nodos[i].etiqueta_a,
				nodos[i].etiqueta_b);
	}

	fclose(salida);
	return 1;
}

// Guarda la matriz de adyacencia en texto.
// Convencion de salida:
//0 = no hay arista
//1 = hay arista directa entre nodos
static int guardar_matriz(const char *archivo, int ady[MAX_NODOS][MAX_NODOS], int n) {
	FILE *salida = fopen(archivo, "w");

	if (salida == NULL) {
		return 0;
	}

	fprintf(salida, " ");
	for (int j = 0; j < n; j++) {
		fprintf(salida, " N%d", j + 1);
	}
	fprintf(salida, "\n");

	for (int i = 0; i < n; i++) {
		fprintf(salida, "N%d", i + 1);
		for (int j = 0; j < n; j++) {
			if (ady[i][j] >= INF) {
				fprintf(salida, " 0");
			} else {
				fprintf(salida, " %d", ady[i][j]);
			}
		}
		fprintf(salida, "\n");
	}

	fclose(salida);
	return 1;
}

// Muestra la ruta completa para visitar todos los destinos en orden.
// Para cada tramo i -> i+1, calcula el camino con Dijkstra y lo imprime en consola.
static void imprimir_ruta(Destino destinos[],
						int cantidad_destinos,
						Nodo nodos[],
						int cantidad_nodos,
						int ady[MAX_NODOS][MAX_NODOS],
						int calle_arista[MAX_NODOS][MAX_NODOS],
						Calle calles[]) {
	int visitado_destino[MAX_DESTINOS];
	int indice_nodo_destino[MAX_DESTINOS];
	int destino_actual;
	int tramos_generados = 0;

	if (cantidad_destinos < 2) {
		printf("No hay suficientes destinos para generar ruta completa.\n");
		return;
	}

	for (int i = 0; i < cantidad_destinos; i++) {
		visitado_destino[i] = 0;
		indice_nodo_destino[i] = indice_nodo_por_coord(nodos, cantidad_nodos, destinos[i].x, destinos[i].y);
		if (indice_nodo_destino[i] < 0) {
			printf("El destino %s (%.2f %.2f) no existe como nodo, no se puede completar ruta.\n",
				   destinos[i].nombre,
				   destinos[i].x,
				   destinos[i].y);
			return;
		}
	}

	visitado_destino[0] = 1;
	destino_actual = 0;

	printf("Ruta completa por destinos en orden:\n\n");

	while (1) {
		int siguiente_objetivo = -1;

		for (int i = destino_actual + 1; i < cantidad_destinos; i++) {
			if (!visitado_destino[i]) {
				siguiente_objetivo = i;
				break;
			}
		}

		if (siguiente_objetivo < 0) {
			break;
		}

		{
		int idx_inicio = indice_nodo_destino[destino_actual];
		int idx_fin = indice_nodo_destino[siguiente_objetivo];
		int camino[MAX_NODOS];
		int largo_camino = 0;
		int existe_camino;

		printf("Tramo %d: %s (%.2f %.2f) -> %s (%.2f %.2f)\n",
		       tramos_generados + 1,
		       destinos[destino_actual].nombre,
		       destinos[destino_actual].x,
		       destinos[destino_actual].y,
		       destinos[siguiente_objetivo].nombre,
		       destinos[siguiente_objetivo].x,
		       destinos[siguiente_objetivo].y);

		existe_camino = dijkstra_reutilizado(cantidad_nodos, ady, idx_inicio, idx_fin, camino, &largo_camino);

		if (!existe_camino || largo_camino == 0) {
			printf("No hay camino entre estos destinos.\n\n");
			printf("No se puede completar la visita de todos los destinos.\n");
			return;
		}

		printf("Camino (nodos numerados): ");
		for (int i = 0; i < largo_camino; i++) {
			printf("N%d", camino[i] + 1);
			if (i < largo_camino - 1) {
				printf(" -> ");
			}
		}
		printf("\n");

		printf("Calles recorridas: ");
		if (largo_camino == 1) {
			printf("sin desplazamiento\n");
		} else {
			int primera = 1;
			int calle_prev = -1;
			for (int i = 0; i < largo_camino - 1; i++) {
				int c = calle_arista[camino[i]][camino[i + 1]];
				if (c >= 0 && c != calle_prev) {
					if (!primera) {
						printf(" -> ");
					}
					printf("%s", calles[c].nombre);
					primera = 0;
					calle_prev = c;
				}
			}
			if (primera) {
				printf("no identificadas");
			}
			printf("\n");
		}

		printf("Detalle del recorrido:\n");
		for (int i = 0; i < largo_camino; i++) {
			int idx = camino[i];
			printf("N%d %.2f %.2f\n", idx + 1, nodos[idx].x, nodos[idx].y);
			for (int d = 0; d < cantidad_destinos; d++) {
				if (casi_igual(destinos[d].x, nodos[idx].x) && casi_igual(destinos[d].y, nodos[idx].y)) {
					visitado_destino[d] = 1;
				}
			}
		}

		printf("Finaliza tramo en N%d\n\n", idx_fin + 1);
		destino_actual = siguiente_objetivo;
		tramos_generados++;
		}
	}

	printf("Ruta completada: se alcanzaron los %d destinos en orden (Si se paso por un destino ya visitado, omitimos su mencion).\n",
		   cantidad_destinos);
}

// Flujo principal del programa:
// 1) Leer calles, nodos y los destinos correspondientes.
// 2) Construir adyacencia de nodos segun las calles.
// 3) Construir adyacencia de nodos segun las calles.
// 4) Ejecutar Dijkstra por tramos para visitar todos los destinos en orden.
// 5) Exportar nodos numerados y matriz para verlo.
int ejecutar_obtener_ruta(void) {
	Calle calles[MAX_CALLES];
	Nodo nodos[MAX_NODOS];
	Destino destinos[MAX_DESTINOS];
	// Estas matrices son grandes, por eso son static (evita overflow de stack).
	static int ady[MAX_NODOS][MAX_NODOS];
	static int calle_arista[MAX_NODOS][MAX_NODOS];
	int cantidad_calles = 0;
	int cantidad_nodos = 0;
	int cantidad_destinos = 0;

	if (!leer_calles("1calles_guardadas.txt", calles, &cantidad_calles)) {
		return 1;
	}

	if (!leer_nodos("4nodos.txt", nodos, &cantidad_nodos)) {
		return 1;
	}

	if (!leer_destinos("3coordenadas_guardadas.txt", destinos, &cantidad_destinos)) {
		return 1;
	}

	// Se necesitan al menos 2 destinos para poder calcular tramos.
	if (cantidad_destinos < 2) {
		printf("Se necesitan al menos 2 destinos para obtener la ruta.\n");
		return 1;
	}

	construir_adyacencia(calles, cantidad_calles, nodos, cantidad_nodos, ady, calle_arista);

	if (!guardar_nodos_numerados("7nodos_numerados.txt", nodos, cantidad_nodos)) {
		printf("No se pudo generar 7nodos_numerados.txt\n");
		return 1;
	}

	if (!guardar_matriz("8matriz_adyacencia.txt", ady, cantidad_nodos)) {
		printf("No se pudo generar 8matriz_adyacencia.txt\n");
		return 1;
	}

	printf("Archivo generado: 7nodos_numerados.txt\n");
	printf("Archivo generado: 8matriz_adyacencia.txt\n\n\n\n\n\n");
	imprimir_ruta(destinos,
				  cantidad_destinos,
				  nodos,
				  cantidad_nodos,
				  ady,
				  calle_arista,
				  calles);

	return 0;
}
