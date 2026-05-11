#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    char nombre_calle[100];
    int x1;
    int y1;
    int x2;
    int y2;
    char eje;
} Calle;

typedef struct {
    int x;
    int y;
    char calle_a[100];
    char calle_b[100];
    int visitado;
} Nodo;

typedef struct {
    char nombre[100];
    int x;
    int y;
    int visitado;
} NodoDestino;

// Valor absoluto pq son coordenadas
static double valor_absoluto(double x) {
    if (x < 0.0) {
        return -x;
    }
    return x;
}

// Compara dos reales con tolerancia para evitar errores por precision decimal.
static int casi_igual(double a, double b) {
    const double eps = 1e-9;
    return valor_absoluto(a - b) < eps;
}

// Calcula el menor entre dos enteros.
static int minimo_int(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

// Calcula el mayor entre dos enteros.
static int maximo_int(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

// Calcula el producto cruzado 2D entre dos vectores.
static double producto_cruz(double ax, double ay, double bx, double by) {
    return ax * by - ay * bx;
}

// Verifica si un punto pertenece al tramo delimitado por una calle.
static int punto_en_segmento(double px, double py, Calle c) {
    double min_x;
    double max_x;
    double min_y;
    double max_y;
    const double eps = 1e-9;

    if (c.x1 < c.x2) {
        min_x = (double)c.x1;
    } else {
        min_x = (double)c.x2;
    }

    if (c.x1 > c.x2) {
        max_x = (double)c.x1;
    } else {
        max_x = (double)c.x2;
    }

    if (c.y1 < c.y2) {
        min_y = (double)c.y1;
    } else {
        min_y = (double)c.y2;
    }

    if (c.y1 > c.y2) {
        max_y = (double)c.y1;
    } else {
        max_y = (double)c.y2;
    }

    if (px < min_x - eps || px > max_x + eps || py < min_y - eps || py > max_y + eps) {
        return 0;
    }

    return 1;
}

// Agrega un nodo nuevo SOLO si esa coordenada no existe todavía.
static int agregar_nodo_si_no_existe(Nodo nodos[], int *cantidad_nodos, int x, int y, const char *a, const char *b) {
    for (int i = 0; i < *cantidad_nodos; i++) {
        if (nodos[i].x == x && nodos[i].y == y) {
            return 1;
        }
    }

    if (*cantidad_nodos >= 1000) {
        return 0;
    }

    nodos[*cantidad_nodos].x = x;
    nodos[*cantidad_nodos].y = y;
    strncpy(nodos[*cantidad_nodos].calle_a, a, sizeof(nodos[*cantidad_nodos].calle_a) - 1);
    nodos[*cantidad_nodos].calle_a[sizeof(nodos[*cantidad_nodos].calle_a) - 1] = '\0';
    strncpy(nodos[*cantidad_nodos].calle_b, b, sizeof(nodos[*cantidad_nodos].calle_b) - 1);
    nodos[*cantidad_nodos].calle_b[sizeof(nodos[*cantidad_nodos].calle_b) - 1] = '\0';
    nodos[*cantidad_nodos].visitado = 0;
    (*cantidad_nodos)++;
    return 1;
}

// Calcula la interseccion entre dos calles y registra el nodo si existe.
static int interseccion_segmentos(Calle a, Calle b, Nodo nodos[], int *cantidad_nodos) {
    double p_x = (double)a.x1;
    double p_y = (double)a.y1;
    double r_x = (double)(a.x2 - a.x1);
    double r_y = (double)(a.y2 - a.y1);

    double q_x = (double)b.x1;
    double q_y = (double)b.y1;
    double s_x = (double)(b.x2 - b.x1);
    double s_y = (double)(b.y2 - b.y1);

    double rxs = producto_cruz(r_x, r_y, s_x, s_y);
    double q_p_x = q_x - p_x;
    double q_p_y = q_y - p_y;
    double q_pxr = producto_cruz(q_p_x, q_p_y, r_x, r_y);

    if (!casi_igual(rxs, 0.0)) {
        double t = producto_cruz(q_p_x, q_p_y, s_x, s_y) / rxs;
        double u = producto_cruz(q_p_x, q_p_y, r_x, r_y) / rxs;

        if (t >= -1e-9 && t <= 1.0 + 1e-9 && u >= -1e-9 && u <= 1.0 + 1e-9) {
            double ix = p_x + t * r_x;
            double iy = p_y + t * r_y;
            int ix_entero = (int)llround(ix);
            int iy_entero = (int)llround(iy);
            return agregar_nodo_si_no_existe(nodos, cantidad_nodos, ix_entero, iy_entero, a.nombre_calle, b.nombre_calle);
        }
        return 1;
    }

    if (!casi_igual(q_pxr, 0.0)) {
        return 1;
    }

    if (punto_en_segmento((double)a.x1, (double)a.y1, b)) {
        if (!agregar_nodo_si_no_existe(nodos, cantidad_nodos, a.x1, a.y1, a.nombre_calle, b.nombre_calle)) {
            return 0;
        }
    }
    if (punto_en_segmento((double)a.x2, (double)a.y2, b)) {
        if (!agregar_nodo_si_no_existe(nodos, cantidad_nodos, a.x2, a.y2, a.nombre_calle, b.nombre_calle)) {
            return 0;
        }
    }
    if (punto_en_segmento((double)b.x1, (double)b.y1, a)) {
        if (!agregar_nodo_si_no_existe(nodos, cantidad_nodos, b.x1, b.y1, a.nombre_calle, b.nombre_calle)) {
            return 0;
        }
    }
    if (punto_en_segmento((double)b.x2, (double)b.y2, a)) {
        if (!agregar_nodo_si_no_existe(nodos, cantidad_nodos, b.x2, b.y2, a.nombre_calle, b.nombre_calle)) {
            return 0;
        }
    }

    return 1;
}

// Lee coordenadas de destinos desde coordenadas_guardadas.txt.
static int leer_destinos_coordenadas(const char *archivo, NodoDestino destinos[], int *cantidad_destinos) {
    FILE *entrada = fopen(archivo, "r");
    char linea[256];
    int contador = 0;

    if (entrada == NULL) {
        printf("No se pudo abrir '%s'.\n", archivo);
        return 0;
    }

    while (fgets(linea, sizeof(linea), entrada) != NULL && contador < 200) {
        if (sscanf(linea,
                   "%99s %d %d",
                   destinos[contador].nombre,
                   &destinos[contador].x,
                   &destinos[contador].y) == 3 ||
            sscanf(linea,
                   "%99s (%d, %d)",
                   destinos[contador].nombre,
                   &destinos[contador].x,
                   &destinos[contador].y) == 3) {
            destinos[contador].visitado = 0;
            contador++;
        }
    }

    fclose(entrada);
    *cantidad_destinos = contador;
    return 1;
}

// Verifica si un punto entero esta sobre el segmento AB.
static int punto_en_segmento_entero(int px, int py, int ax, int ay, int bx, int by) {
    long long abx = (long long)bx - (long long)ax;
    long long aby = (long long)by - (long long)ay;
    long long apx = (long long)px - (long long)ax;
    long long apy = (long long)py - (long long)ay;
    long long cruz = abx * apy - aby * apx;

    if (cruz != 0) {
        return 0;
    }

    if (px < minimo_int(ax, bx) || px > maximo_int(ax, bx)) {
        return 0;
    }

    if (py < minimo_int(ay, by) || py > maximo_int(ay, by)) {
        return 0;
    }

    return 1;
}

// Marca nodos y destinos visitados al avanzar entre dos destinos en orden.
static void marcar_recorrido(int origen_x,
                             int origen_y,
                             int destino_x,
                             int destino_y,
                             Nodo nodos[],
                             int cantidad_nodos,
                             NodoDestino destinos[],
                             int cantidad_destinos,
                             int indice_minimo) {
    for (int i = 0; i < cantidad_nodos; i++) {
        if (punto_en_segmento_entero(nodos[i].x, nodos[i].y, origen_x, origen_y, destino_x, destino_y)) {
            nodos[i].visitado = 1;
        }
    }

    for (int i = indice_minimo; i < cantidad_destinos; i++) {
        if (punto_en_segmento_entero(destinos[i].x, destinos[i].y, origen_x, origen_y, destino_x, destino_y)) {
            destinos[i].visitado = 1;
        }
    }
}

// Recorre destinos en orden creciente, saltando los ya visitados previamente.
static void recorrer_destinos_en_orden(Nodo nodos[], int cantidad_nodos, NodoDestino destinos[], int cantidad_destinos) {
    int actual_x;
    int actual_y;
    int siguiente;

    if (cantidad_destinos <= 0) {
        return;
    }

    destinos[0].visitado = 1;
    actual_x = destinos[0].x;
    actual_y = destinos[0].y;
    siguiente = 1;

    while (siguiente < cantidad_destinos) {
        while (siguiente < cantidad_destinos && destinos[siguiente].visitado == 1) {
            siguiente++;
        }

        if (siguiente >= cantidad_destinos) {
            break;
        }

        marcar_recorrido(actual_x,
                         actual_y,
                         destinos[siguiente].x,
                         destinos[siguiente].y,
                         nodos,
                         cantidad_nodos,
                         destinos,
                         cantidad_destinos,
                         siguiente);

        destinos[siguiente].visitado = 1;
        actual_x = destinos[siguiente].x;
        actual_y = destinos[siguiente].y;
        siguiente++;
    }
}

// Agrega cada destino como nodo (si no existe ya por interseccion).
static int agregar_destinos_como_nodos(Nodo nodos[], int *cantidad_nodos, NodoDestino destinos[], int cantidad_destinos) {
    for (int i = 0; i < cantidad_destinos; i++) {
        if (!agregar_nodo_si_no_existe(nodos,
                                       cantidad_nodos,
                                       destinos[i].x,
                                       destinos[i].y,
                                       "DESTINO",
                                       destinos[i].nombre)) {
            return 0;
        }
    }

    return 1;
}

// Guarda el estado final de los nodos con su marca de visitado.
static int guardar_estado_nodos(const char *archivo, Nodo nodos[], int cantidad_nodos) {
    FILE *salida = fopen(archivo, "w");

    if (salida == NULL) {
        printf("No se pudo abrir '%s' para escritura.\n", archivo);
        return 0;
    }

    for (int i = 0; i < cantidad_nodos; i++) {
        fprintf(salida,
                "%s %s %d %d %d\n",
                nodos[i].calle_a,
                nodos[i].calle_b,
                nodos[i].x,
                nodos[i].y,
                nodos[i].visitado);
    }

    fclose(salida);
    return 1;
}

// Guarda el estado final de los destinos con su marca de visitado.
static int guardar_estado_destinos(const char *archivo, NodoDestino destinos[], int cantidad_destinos) {
    FILE *salida = fopen(archivo, "w");

    if (salida == NULL) {
        printf("No se pudo abrir '%s' para escritura.\n", archivo);
        return 0;
    }

    for (int i = 0; i < cantidad_destinos; i++) {
        fprintf(salida,
                "%d %s %d %d %d\n",
                i + 1,
                destinos[i].nombre,
                destinos[i].x,
                destinos[i].y,
                destinos[i].visitado);
    }

    fclose(salida);
    return 1;
}

// Lee calles desde 1calles_guardadas.txt y carga el arreglo en memoria.
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
                   &calles[contador].x1,
                   &calles[contador].y1,
                   &calles[contador].x2,
                   &calles[contador].y2,
                   &calles[contador].eje) != 6) {
            printf("Formato invalido en calle %d.\n", contador + 1);
            fclose(entrada);
            return 0;
        }
        contador++;
    }

    fclose(entrada);
    *cantidad_calles = contador;
    return 1;
}

// Programa principal que calcula intersecciones y las guarda en nodos.txt.
int ejecutar_obtener_nodos(void) {
    Calle calles[50];
    Nodo nodos[1000];
    NodoDestino destinos[200];
    int cantidad_calles = 0;
    int cantidad_nodos = 0;
    int cantidad_destinos = 0;
    FILE *salida;

    if (!leer_calles("1calles_guardadas.txt", calles, &cantidad_calles)) {
        return 1;
    }

    for (int i = 0; i < cantidad_calles; i++) {
        for (int j = i + 1; j < cantidad_calles; j++) {
            if (!interseccion_segmentos(calles[i], calles[j], nodos, &cantidad_nodos)) {
                printf("Se alcanzo el limite de nodos.\n");
                return 1;
            }
        }
    }

    if (!leer_destinos_coordenadas("3coordenadas_guardadas.txt", destinos, &cantidad_destinos)) {
        return 1;
    }

    if (!agregar_destinos_como_nodos(nodos, &cantidad_nodos, destinos, cantidad_destinos)) {
        printf("Se alcanzo el limite de nodos al agregar destinos.\n");
        return 1;
    }

    recorrer_destinos_en_orden(nodos, cantidad_nodos, destinos, cantidad_destinos);

    salida = fopen("4nodos.txt", "w");
    if (salida == NULL) {
        printf("No se pudo abrir '4nodos.txt' para escritura.\n");
        return 1;
    }

    for (int i = 0; i < cantidad_nodos; i++) {
        fprintf(salida,
                "%s %s %d %d\n",
                nodos[i].calle_a,
                nodos[i].calle_b,
                nodos[i].x,
                nodos[i].y);
    }

    fclose(salida);

    printf("Nodos encontrados: %d\n", cantidad_nodos);
    printf("Archivo generado: 4nodos.txt\n");
    if (!guardar_estado_nodos("5estado_nodos.txt", nodos, cantidad_nodos)) {
        return 1;
    }
    if (!guardar_estado_destinos("6estado_destinos.txt", destinos, cantidad_destinos)) {
        return 1;
    }
    printf("Archivo generado: 5estado_nodos.txt\n");
    printf("Archivo generado: 6estado_destinos.txt\n");
    return 0;
}