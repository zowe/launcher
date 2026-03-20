/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

/*
 * dep_graph.c - Dependency graph utilities for the Zowe Launcher.
 *
 * Implements dep_topo_sort() using Kahn's algorithm (BFS-based topological
 * sort).  This file has no dependencies on launcher-specific types and can
 * be compiled and tested in isolation.
 */

#include <string.h>
#include "dep_graph.h"

int dep_topo_sort(int n, const int *depends, int *order) {

  if (n <= 0 || n > DEP_GRAPH_MAX_NODES) {
    return -1;
  }

  /* in_degree[i] = number of nodes that i depends on (i.e. how many
   * other nodes must become ready before i can start). */
  int in_degree[DEP_GRAPH_MAX_NODES];
  memset(in_degree, 0, sizeof(in_degree));

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (depends[i * n + j]) {
        in_degree[i]++;
      }
    }
  }

  /* Queue of nodes whose in-degree has reached 0 (ready to place in order). */
  int queue[DEP_GRAPH_MAX_NODES];
  int q_head = 0;
  int q_tail = 0;

  for (int i = 0; i < n; i++) {
    if (in_degree[i] == 0) {
      queue[q_tail++] = i;
    }
  }

  int order_idx = 0;

  while (q_head < q_tail) {
    int node = queue[q_head++];
    order[order_idx++] = node;

    /* For every node that depends on 'node', decrement its in-degree.
     * When in-degree reaches 0 that node's dependencies are all accounted
     * for and it can be enqueued. */
    for (int i = 0; i < n; i++) {
      if (depends[i * n + node]) {
        if (--in_degree[i] == 0) {
          queue[q_tail++] = i;
        }
      }
    }
  }

  /* If we didn't place all nodes there is a cycle. */
  if (order_idx != n) {
    return -1;
  }

  return 0;
}

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
