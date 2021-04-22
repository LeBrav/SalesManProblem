#include "pch.h"
#include "Graph.h"
#include <set>


// =============================================================================
// SalesmanTrackBacktracking ===================================================
// =============================================================================
double best_distance = numeric_limits<double>::max();
double distance0 = 0;
CTrack best_cami(NULL);
CTrack cami(NULL);
CVertex* desti;

struct NodeCami {
	CEdge* m_pEdge;
	NodeCami* m_pAnterior;
};

double get_distance(CTrack& cami)
{
	double distance0 = 0;
	for (CEdge* pE : cami.m_Edges)
		distance0 += pE->m_Length;
	return distance0;
}

bool cami_correcte(CVisits& visits, CTrack& cami)
{
	for (CVertex* v : visits.m_Vertices)
	{
		if (v != visits.m_Vertices.back())
		{
			bool correcte = false;
			for (CEdge* pE : v->m_Edges)
			{
				if (pE->m_Processed)
				{
					correcte = true;
					break;
				}
			}
			if (!correcte)
				return false;
		}

	}
	return true;

}


void SalesmanTrackBacktrackingRec(CVertex* v, CVisits& visits, NodeCami* pAnterior)
{
	if (desti == v)//si el vertex actual es el desti
	{
		if (best_distance > distance0)//es millor que el anterior millor cami
		{
			if (cami_correcte(visits, cami)) //hem mriat totes les visites
			{//actualitzem millor solucio
				best_distance = distance0;
				best_cami.Clear();
				while (pAnterior) {
					best_cami.m_Edges.push_front(pAnterior->m_pEdge);
					pAnterior = pAnterior->m_pAnterior;
				}
			}
		}
	}
	else
	{
		for (CEdge* pE : v->m_Edges) //explorem cada aresta del vertex actual
		{
			//SI ARESTA NO VISITADA ANTERIORMENT (la daquest sentit) i anar alla sera mes petit que la millor solucio (poda), anem
			if (!pE->m_Processed && best_distance > distance0 + pE->m_Length) 
			{
				//afegim a la solucio actual del backtracking aquest nou vertex i cami i marquem aresta com aprocesada
				NodeCami node;
				node.m_pAnterior = pAnterior;
				node.m_pEdge = pE;

				pE->m_Processed = true;
				distance0 += pE->m_Length;

				//fem en recursio el backtracking
				SalesmanTrackBacktrackingRec(pE->m_pDestination, visits, &node);

				//desfem aquest vertex i cami 
				pE->m_Processed = false;
				distance0 -= pE->m_Length;
			}
		}
	}
}

CTrack SalesmanTrackBacktracking(CGraph& graph, CVisits& visits)
{
	for (CEdge& pE : graph.m_Edges)
		pE.m_Processed = false;

	CVertex* v = visits.m_Vertices.front(); //Sigui v la primera visita de la llista de visites
	desti = visits.m_Vertices.back();
	best_cami.Clear();
	cami.Clear();
	cami.SetGraph(&graph);
	if (!v->m_Edges.empty())
		SalesmanTrackBacktrackingRec(v, visits, NULL);

	return best_cami;
}


// =============================================================================
// SalesmanTrackBacktrackingGreedy =============================================
// =============================================================================

bool cami_correcte_2(CVisits& visits, CTrack& cami) 
{
	for (CVertex* v : visits.m_Vertices)
	{
		bool correcte = false;
		for (CEdge* pE : cami.m_Edges)
		{
			if (pE->m_pOrigin == v || pE->m_pDestination == v)
			{
				correcte = true;
				break;
			}
		}
		if (!correcte)
			return false;
	}
	return true;
}

//utilitzar l’algorisme de Dijkstra per trobar camins entre visites
//• utilitzar backtracking per decidir l’ordre de les visites

void SalesmanTrackBacktrackingGreedyRec(CGraph& graph, CVisits& visits, CVertex* v, list<CVertex*> candidats_aux)
{

	CVertex* destiF = visits.m_Vertices.back();


	for (CVertex* v1 : candidats_aux) //explorem cada visita desdel vertex actual
	{
		if (v1 != destiF || candidats_aux.size() <= 1) { //Si on estem encara no es el desti o si estem al desti amb 1 candidat left nomes ENTRA
			DijkstraQueue(graph, v);
			distance0 = get_distance(cami); //distance del cami actual
			if (!v1->m_Marca && best_distance > distance0 + v1->m_DijkstraDistance) //vertex sense visitar y a més el cami és més curt que el millor fins ara
			{
				//treiem de candidats el vertex v1, es a dir, la visita qeue stem explorant
				list<CVertex*> candidats_aux2 = candidats_aux;
				list<CVertex*> candidats_aux;
				for (CVertex* vaux : candidats_aux2) {
					if (vaux != v1) candidats_aux.push_back(vaux);
				}

				v1->m_Marca = true;

				//Update the path with this visit
				CTrack cami_aux(&graph);
				CTrack cami_aux2 = cami;
				cami_aux = cami_aux.CamiOptim(graph, v, v1);
				cami.Append(cami_aux);
				double suma_dist = v1->m_DijkstraDistance;

				if (v1 == destiF)//possible solució si estem en el desti
				{
					if (cami_correcte_2(visits, cami)) { //i si totes les visites visitades INECESSARI JA QUE ES MIRA AL PRIMER IF DE TOTS
						best_distance = distance0 + suma_dist;
						best_cami = cami;
					}
				}
				else //si no es el desti encara anem en recursiu exploran
					SalesmanTrackBacktrackingGreedyRec(graph, visits, v1, candidats_aux);

				//desfer aquella visita
				candidats_aux = candidats_aux2;
				cami = cami_aux2;
				v1->m_Marca = false;
			}
		}
	}
}

//utilitzar l’algorisme de Dijkstra per trobar camins entre visites
//• utilitzar backtracking per decidir l’ordre de les visites
CTrack SalesmanTrackBacktrackingGreedy(CGraph& graph, CVisits& visits)
{

	distance0 = 0;
	best_distance = numeric_limits<double>::max();
	CTrack aux(&graph);
	best_cami = aux;
	cami = aux;


	for (CVertex& v : graph.m_Vertices)
		v.m_Marca = false;
	for (CVertex& v : graph.m_Vertices)
		v.m_JaHePassat = false;

	for (CEdge& pE : graph.m_Edges)
		pE.m_Processed = false;

	CVertex* v = visits.m_Vertices.front(); //Sigui v la primera visita de la llista de visites
	v->m_Marca = true;

	list<CVertex*> candidats_aux = visits.m_Vertices;
	candidats_aux.pop_front();


	if (!v->m_Edges.empty())
	{
		SalesmanTrackBacktrackingGreedyRec(graph, visits, v, candidats_aux);
	}
	return best_cami;

}