#include "pch.h"
#include "Graph.h"

// SalesmanTrackGreedy =========================================================

CTrack CTrack::CamiOptim(CGraph& graph, CVertex* pActual, CVertex* pDesti)
{
	CVertex* aux_3 = pDesti;
	CVertex* next = pDesti;
	double min_dist = numeric_limits<double>::max();
	CTrack cami_aux(&graph); //Cami desde V a V1

	//ANEM DE V1 A V, i afegim a cami_aux amb push front per convertiro a V->V1
	while (min_dist > 0)//MENTRES NO ESTIGUEM AL FINAL DEL CAMI AUX (A V)
	{
		//Busquem de tots els veins de V1 el m�s proper a el node V (objectiu, amb distance dijkstra = 0),
		//utilitant la dikstra distance + distancia aresta que uneix el Vei amb V1
		CEdge* actualE = pDesti->m_Edges.front();
		for (CEdge* pE : aux_3->m_Edges) {
			if (min_dist >= pE->m_pDestination->m_DijkstraDistance + pE->m_Length) {
				min_dist = pE->m_pDestination->m_DijkstraDistance + pE->m_Length;
				next = pE->m_pDestination;
				actualE = pE;
			}
		}

		//Un cop trobat el seg�ent node a visitar que forma el cami optim aux,
		//igualem la min_dist a la seva m_DijkstraDistance, en cas que sigui min_dist = 0, hem acabat el cami_aux
		min_dist = actualE->m_pDestination->m_DijkstraDistance;
		aux_3 = next;
		cami_aux.m_Edges.push_front(actualE->m_pReverseEdge); //Afegim el primer ve� al cam�_aux
	}
	return cami_aux;
}

CTrack SalesmanTrackGreedy(CGraph& graph, CVisits& visits)
{
	list<CVertex*> candidats = visits.m_Vertices;
	CTrack cami(&graph);

	CVertex *v = candidats.front(); //Sigui v la primera visita de la llista de visites

	//Candidats= tots els v�rtex de la llista de visites menys el primer i l'�ltim
	candidats.pop_front();
	candidats.pop_back();

	while (!candidats.empty()) //Repetir mentre candidats no estigui buit
	{
		CVertex* v1 = v;
		double min_dist = numeric_limits<double>::max();
		DijkstraQueue(graph, v); //Aplicant el algorisme de Dijkstra calcular les distancies de tots els v�rtexs del graf a v.

		//Seleccionar v1 com el v�rtex pertanyent a Candidats amb distancia m�nima.
		for (CVertex *aux : candidats) {
			if (min_dist >= aux->m_DijkstraDistance) {
				min_dist = aux->m_DijkstraDistance;
				v1 = aux;
			}
			
		}
		
		//Afegir el cam� de v a v1 al resultat.-----------------------------------
		CTrack cami_aux(&graph); //Cami desde V a V1
		cami_aux = cami_aux.CamiOptim(graph, v, v1);
		cami.Append(cami_aux); //Sumem el cami que ja portavem amb el cami de V1 a V
		//-----------------------------------------------------------------------
		
		candidats.remove(v1); //Treure v1 de Candidats

		v = v1; //El nou vertex start es el v1, vertex al qual acabem d'arribar
		
	}
	
	//Anar del �ltim candidat seleccionat al v�rtex final de la llista de visites. 
	DijkstraQueue(graph, v); //Aplicant el algorisme de Dijkstra calcular les distancies de tots els v�rtexs del graf a v.

	//Afegir el cam� de v a Final al resultat.-----------------------------------
	CTrack cami_aux(&graph); //Cami desde V a Final
	cami_aux = cami_aux.CamiOptim(graph, v, visits.m_Vertices.back());
	cami.Append(cami_aux); //Sumem el cami que ja portavem amb el cami de Final a V
	//-----------------------------------------------------------------------

	return cami;
}
