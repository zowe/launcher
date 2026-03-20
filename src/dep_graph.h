/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

/*
 * dep_graph.h - Dependency graph utilities for the Zowe Launcher.
 *
 * Provides a generic topological sort used to determine component startup
 * order based on declared inter-component dependencies.
 *
 * The graph is represented as a flat adjacency matrix:
 *   depends[i * n + j] == 1   means node i depends on node j
 *                              (i must start AFTER j is ready)
 *
 * dep_topo_sort() implements Kahn's algorithm.  On success it fills 'order'
 * with the indices of the n nodes in a valid start sequence (dependencies
 * first) and returns 0.  If a cycle is detected it returns -1; the caller
 * should fall back to declaration order and emit a warning.
 */

#ifndef DEP_GRAPH_H
#define DEP_GRAPH_H

/* Maximum number of nodes supported by this implementation.
 * Must be >= MAX_CHILD_COUNT defined in main.c (currently 128). */
#define DEP_GRAPH_MAX_NODES 128

/*
 * dep_topo_sort - topological sort of a dependency graph.
 *
 * Parameters:
 *   n        Number of nodes (must be <= DEP_GRAPH_MAX_NODES).
 *   depends  Row-major n×n integer matrix allocated by the caller.
 *            depends[i*n + j] = 1 if node i depends on node j.
 *   order    Output array of length n.  On success contains the node
 *            indices in a valid start order (index 0 = first to start).
 *
 * Returns 0 on success, -1 if a cycle is detected.
 */
int dep_topo_sort(int n, const int *depends, int *order);

#endif /* DEP_GRAPH_H */

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
