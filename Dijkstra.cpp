#include "pch.h"
#include "Graph.h"
#include <queue>
#include <vector>
#include <tuple>


// =============================================================================
// Dijkstra ====================================================================
// =============================================================================


struct comparator {
	bool operator()(CVertex* v1, CVertex* v2) { return v1->m_DijkstraDistance >= v2->m_DijkstraDistance; }
};

void Dijkstra(CGraph& graph, CVertex* pStart)
{
	for (CVertex& v : graph.m_Vertices) {
		v.m_DijkstraDistance = numeric_limits<double>::max(); //Inicialitzar les distancies dels v�rtex a infinit excepte la del v�rtex pStart que ser� 0
		v.m_JaHePassat = false; //Marcar tots el v�rtex com no visitats
	}
	pStart->m_DijkstraDistance = 0;

	CVertex* pActual = pStart;
	if (!pActual->m_Edges.empty()) //en cas de ser un graqf poxo k no hi ha res
	{
		for (int i = 0; i < graph.GetNVertices(); i++) { //es fara una vegada per cada vertex que es el mteix que mirar while !tots visitats
			for (CEdge* pE : pActual->m_Edges) { //per cada vei del actual mirem si es mes rapid per on estem

				//Si la distancia de v es m�s grossa que la distancia del v�rtex actual m�s la longitud de l�aresta que els uneix
				if (pE->m_pDestination->m_DijkstraDistance > pActual->m_DijkstraDistance + pE->m_Length)
					pE->m_pDestination->m_DijkstraDistance = pActual->m_DijkstraDistance + pE->m_Length; //actualitzar la distancia de v amb distancia de va m�s la longitud de la aresta que els uneix.
			}
			pActual->m_JaHePassat = true;

			double min_dist = numeric_limits<double>::max();
			for (CVertex& v : graph.m_Vertices) {//pActual=v�rtex no visitat amb distancia m�s petita i tornar a repetir el bucle
				if (min_dist >= v.m_DijkstraDistance && !v.m_JaHePassat) {
					min_dist = v.m_DijkstraDistance;
					pActual = &v;
				}
			}
		}
	}
}

// =============================================================================
// DijkstraQueue ===============================================================
// =============================================================================

void DijkstraQueue(CGraph& graph, CVertex* pStart)
{
	priority_queue<CVertex*, vector<CVertex*>, comparator> queue;
	for (CVertex& v : graph.m_Vertices) {
		v.m_DijkstraDistance = numeric_limits<double>::max(); //Inicialitzar les distancies dels v�rtex a infinit excepte la del v�rtex pStart que ser� 0
		v.m_JaHePassat = false; //Marcar tots el v�rtex com no visitats
	}
	pStart->m_DijkstraDistance = 0;

	queue.push(pStart);

	while (!queue.empty()) {
		CVertex* pActual = queue.top();
		queue.pop();
		if (!pActual->m_JaHePassat)
		{
			for (CEdge* pE : pActual->m_Edges) {
				//Si la distancia de v es m�s grossa que la distancia del v�rtex actual m�s la longitud de l�aresta que els uneix
				if (pE->m_pDestination->m_DijkstraDistance > pActual->m_DijkstraDistance + pE->m_Length) {
					pE->m_pDestination->m_DijkstraDistance = pActual->m_DijkstraDistance + pE->m_Length; //actualitzar la distancia de v amb distancia de va m�s la longitud de la aresta que els uneix.
					queue.push(pE->m_pDestination);
				}
			}
			pActual->m_JaHePassat = true;
		}
	}
}

/*
priority_queue<CVertex*, vector<CVertex*>, comparator> queue;
	for (CVertex& v : graph.m_Vertices) {
		v.m_DijkstraDistance = numeric_limits<double>::max(); //Inicialitzar les distancies dels v�rtex a infinit excepte la del v�rtex pStart que ser� 0
		v.m_DijkstraVisit = false; //Marcar tots el v�rtex com no visitats
		queue.push(&v);
	}
	pStart->m_DijkstraDistance = 0;

	CVertex *pActual = pStart;

	while (!graph_tot_visitat(graph, pStart)) {
		for (CEdge* pE : pActual->m_Edges) {
			//Si la distancia de v es m�s grossa que la distancia del v�rtex actual m�s la longitud de l�aresta que els uneix
			if (pE->m_pDestination->m_DijkstraDistance > pActual->m_DijkstraDistance + pE->m_Length) {
				pE->m_pDestination->m_DijkstraDistance = pActual->m_DijkstraDistance + pE->m_Length; //actualitzar la distancia de v amb distancia de va m�s la longitud de la aresta que els uneix.
				queue.push(pE->m_pDestination);
			}
		}
		pActual->m_DijkstraVisit = true;
		pActual = queue.top();
		queue.pop();
	}

*/