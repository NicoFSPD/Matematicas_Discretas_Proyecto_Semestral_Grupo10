# Proyecto Matemáticas Discretas — Grupo 10

## Integrantes
- Francisco Fuentealba
- Nicolás Silva

---

# Descripción

Este proyecto consiste en la implementación de un sistema de rutas turísticas utilizando teoría de grafos.  

El programa recibe como entrada un conjunto de calles y destinos turísticos, modelando la ciudad como un grafo donde:

- Las intersecciones actúan como nodos.
- Las calles actúan como aristas.
- Los destinos turísticos también se incorporan como nodos del sistema.

Posteriormente, se construye una matriz de adyacencia y se aplica un algoritmo de búsqueda de caminos basado en Dijkstra/BFS para encontrar rutas válidas entre los destinos.

---

# Estructura del Proyecto

El programa está compuesto por los siguientes archivos:

- `Tarea_Semestral.c`
- `extraer_calles_y_destinos.c`
- `mapa.c`
- `obtener_nodos.c`
- `obtener_ruta.c`

---

# Instrucciones de Compilación

Compilar utilizando:

```bash
cd Matemáticas_Discretas_Tarea_Semestral 
gcc ./Tarea_Semestral.c ./extraer_calles_y_destinos.c ./mapa.c ./obtener_nodos.c ./obtener_ruta.c -o ./Tarea_Semestral -lm
```

---

# Ejecución

Ejecutar el programa con:

```bash
./Tarea_Semestral
```

---

# Archivos Generados

Durante la ejecución se generan distintos archivos auxiliares y de diagnóstico:

- `1calles_guardadas.txt`
- `2destinos_guardados.txt`
- `3coordenadas_guardadas.txt`
- `4nodos.txt`
- `5estado_nodos.txt`
- `6destinos_visitados.txt`
- `7nodos_numerados.txt`
- `8matriz_adyacencia.txt`

Estos permiten visualizar el estado interno del programa y verificar la construcción del grafo y de la ruta.

---

# Conceptos Utilizados

- Teoría de Grafos
- Matriz de Adyacencia
- BFS / Dijkstra
- Geometría Analítica
- Intersección de Segmentos
- Manejo de Archivos en C
- Structs y Arreglos

---

# Observaciones

El programa considera:
- Validación de entrada.
- Manejo de calles horizontales, verticales y diagonales.
- Verificación de intersecciones.
- Construcción automática del grafo.
- Generación de rutas entre destinos turísticos.
- IMPORTANTE: Si ejecuta correctamente, los 8 archivos se van a ejecutar, en el caso de que no ocurra, pueden quedar los archivos sueltos, no afectan el rendimiento del programa, pues se reescriben en cada iteración correcta. Ellos están netamente para visualizar el proceso lógico y entender nuestro pensamiento.
