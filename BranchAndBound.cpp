#include "pch.h"
#include "Graph.h"
#include <queue>
#include <iostream>
#include <iomanip> 
#include <ctime>

int SolutionNodesCreated;
int SolutionNodesBranched;

double returnDist(vector<vector<double>> taula, CVisits& visits, CVertex v, CVertex v1)
{
	return taula[v.iter][v1.iter];
}

class CBBNode1 {
public:
	CBBNode1* m_CBBNode;
	CVertex* m_v;
	list<CVertex*> m_Candidats;
	double m_Length;
	double m_cotaInferior; // Cota inferior. peso del nodo a más pesado más largo es su posible camino
	double m_cotaSuperior;
	int m_CntRef;
public:
	CBBNode1(CGraph* pGraph, CVertex* pDestination, list<CVertex*> candidats)
		: m_cotaInferior(0)
		, m_v(pDestination)
		, m_Candidats(candidats)
		, m_cotaSuperior(0)
		, m_CntRef(1)
	{
		//m_nodes.reserve(candidats.size());
		//m_nodes.push_back(pDestination);
	}
	CBBNode1(const CBBNode1& node)
		: m_Candidats(node.m_Candidats)
		, m_cotaInferior(node.m_cotaInferior)
		, m_cotaSuperior(node.m_cotaSuperior)
		, m_CntRef(1)
	{
	}

	void Unlink()
	{
		if (--m_CntRef == 0) {
			if (m_CBBNode) m_CBBNode->Unlink();
			delete this;
		}
	}
};


// TrobaCamiBranchAndBound ======================================================

vector<vector<double>> construir_taula(CGraph& g, CVisits& visits)
{
	list<CVertex*> candidats_no_last = visits.m_Vertices;
	candidats_no_last.pop_back();

	vector<vector<double>> taula;
	for (CVertex* v : candidats_no_last) {
		//Dijkstra(g, v);
		DijkstraQueue(g, v);
		vector<double> taula_c;
		for (CVertex* aux : visits.m_Vertices) {
			taula_c.push_back(aux->m_DijkstraDistance);
		}
		taula.push_back(taula_c);
	}
	return taula;
}

void mostrar_taula(vector<vector<CVertex>> taula)
{
	for (int i = 0; i < taula.size(); i++)
	{
		for (int j = 0; j < taula[i].size(); j++)
		{
			if (taula[i][j].m_DijkstraDistance != 0)
				cout << taula[i][j].m_DijkstraDistance << "       ";
			else
				cout << taula[i][j].m_DijkstraDistance << "              ";
		}
		cout << endl;
	}
}

CTrack construir_track(CBBNode1* solucio, CGraph& g)
{
	CTrack optimum(&g);
	while (solucio->m_CBBNode)
	{
		//Dijkstra(g, solucio->m_v);
		DijkstraQueue(g, solucio->m_v);
		CTrack cami_aux(&g);
		CTrack cami_aux2(&g);
		cami_aux = cami_aux.CamiOptim(g, solucio->m_v, solucio->m_CBBNode->m_v);
		for (CEdge* pE : cami_aux.m_Edges)
		{
			optimum.AddFirst(pE->m_pReverseEdge);
		}
		//optimum.Append(cami_aux2);
		solucio = solucio->m_CBBNode;
	}
	return optimum;
}

// SalesmanTrackBranchAndBound1 ===================================================
struct comparator3 {
	bool operator()(const CBBNode1* s1, const CBBNode1* s2) {
		return s1->m_cotaInferior > s2->m_cotaInferior;
	}
};
CTrack SalesmanTrackBranchAndBound1(CGraph& g, CVisits& visits)
{
	SolutionNodesCreated = 0;
	int i = 0;
	for (CVertex* v0 : visits.m_Vertices)
	{
		v0->iter = i;
		i++;
	}
	visits.m_Vertices.front()->iter = 0;

	list<CVertex*> candidats_no_last = visits.m_Vertices;
	candidats_no_last.pop_back();

	vector<vector<double>> taula = construir_taula(g, visits);

	//mostrar_taula(taula);

	// CALCULEM LA COTA INICIAL, LA DEL NODE INICIAL
	list<CVertex*> candidats_aux = visits.m_Vertices;
	candidats_aux.pop_front();

	CVertex* pInici = visits.m_Vertices.front();
	CVertex* pDesti = visits.m_Vertices.back();

	priority_queue<CBBNode1*, std::vector<CBBNode1*>, comparator3> queue;
	candidats_aux.pop_back();

	CBBNode1* pSNew = new CBBNode1(&g, pInici, candidats_aux);
	pSNew->m_cotaInferior = 0;

	vector<CVertex*> list_optimum;

	if (pSNew->m_Candidats.size() == 0)//simple
	{
		CBBNode1* pSDesti = new CBBNode1(&g, pDesti, candidats_aux);
		pSDesti->m_CBBNode = pSNew;
		pSDesti->m_cotaInferior = pSNew->m_cotaInferior + taula[pInici->iter][pDesti->iter];
		queue.push(pSDesti);
	}
	else
		queue.push(pSNew);

	SolutionNodesCreated++;
	CBBNode1* solucio = nullptr;

	double maxLength = numeric_limits<double>::max();
	while (!queue.empty()) {
		CBBNode1* pS = queue.top();
		queue.pop();

		CVertex* pV = pS->m_v;
		if (pV == pDesti && pS->m_Candidats.size() == 0) {
			// Solucion FINAL encontrada
			solucio = pS;
			break;
		}
		else {
			if (pS->m_Candidats.size() != 0)
			{
				for (CVertex* v : pS->m_Candidats) {
					double w = pS->m_cotaInferior + taula[pV->iter][v->iter];
					if (w < maxLength)
					{
						CBBNode1* pSNew = new CBBNode1(*pS);
						pSNew->m_cotaInferior = w;
						pSNew->m_Candidats.remove(v);
						pSNew->m_v = v;
						pSNew->m_CBBNode = pS;
						queue.push(pSNew);
						SolutionNodesCreated++;
					}
				}
			}
			else
			{
				double w = pS->m_cotaInferior + taula[pS->m_v->iter][pDesti->iter];
				if (w < maxLength)
				{
					CBBNode1* pSNew2 = new CBBNode1(*pS);
					pSNew2->m_CBBNode = pS;
					pSNew2->m_v = pDesti;
					pSNew2->m_cotaInferior = w;
					queue.push(pSNew2);
					SolutionNodesCreated++;
					maxLength = pSNew2->m_cotaInferior;
				}
			}

		}
	}

	CTrack optimum(&g);
	optimum = construir_track(solucio, g);

	//cout << "NODES CREATED: " << SolutionNodesCreated << endl;

	while (!queue.empty()) {
		CBBNode1* pS = queue.top();
		queue.pop();
		pS->Unlink();
	}

	return optimum;
}




CTrack SalesmanTrackBranchAndBound2(CGraph& g, CVisits& visits)
{
	SolutionNodesCreated = 0;
	int i = 0;
	for (CVertex* v0 : visits.m_Vertices)
	{
		v0->iter = i;
		i++;
	}
	visits.m_Vertices.front()->iter = 0;

	list<CVertex*> candidats_no_last = visits.m_Vertices;
	candidats_no_last.pop_back();

	vector<vector<double>> taula = construir_taula(g, visits);

	//mostrar_taula(taula);

	// CALCULEM LA COTA INICIAL, LA DEL NODE INICIAL
	list<CVertex*> candidats_aux = visits.m_Vertices;
	candidats_aux.pop_front();
	double cota_inferior = 0;
	double cota_superior = 0;
	double cota_superiorGlobal = 0;
	for (CVertex* v0 : candidats_aux)
	{
		for (CVertex* v : candidats_no_last)
		{
			double distance = returnDist(taula, visits, *v, *v0);
			if (distance > 0)
				v0->CVi.push_back(distance);
		}
		v0->m_minElement = *min_element(v0->CVi.begin(), v0->CVi.end()); //cvi = camins per on podies anar aquell node, ens quedem la min distancia i la max
		v0->m_maxElement = *max_element(v0->CVi.begin(), v0->CVi.end());
		cota_inferior += v0->m_minElement;
		cota_superior += v0->m_maxElement;
	}
	cota_superiorGlobal = cota_superior;

	CVertex* pInici = visits.m_Vertices.front();
	CVertex* pDesti = visits.m_Vertices.back();

	priority_queue<CBBNode1*, std::vector<CBBNode1*>, comparator3> queue;
	candidats_aux.pop_back();

	CBBNode1* pSNew = new CBBNode1(&g, pInici, candidats_aux);
	pSNew->m_cotaInferior = cota_inferior;
	pSNew->m_cotaSuperior = cota_superior;


	if (pSNew->m_Candidats.size() == 0)//simple
	{
		CBBNode1* pSDesti = new CBBNode1(&g, pDesti, candidats_aux);
		pSDesti->m_CBBNode = pSNew;
		pSDesti->m_cotaInferior = pSNew->m_cotaInferior - pDesti->m_minElement + returnDist(taula, visits, *pInici, *pDesti);
		queue.push(pSDesti);
	}
	else
		queue.push(pSNew);
	SolutionNodesCreated++;
	vector<CVertex*> list_optimum;
	CBBNode1* solucio = nullptr;
	while (!queue.empty()) {
		CBBNode1* pS = queue.top();
		queue.pop();

		CVertex* pV = pS->m_v;
		if (pV == pDesti && pS->m_Candidats.size() == 0) {
			// Solucion FINAL encontrada
			solucio = pS;
			break;
		}
		else {
			if (pS->m_Candidats.size() != 0)
			{
				for (CVertex* v : pS->m_Candidats) {
					CBBNode1* pSNew = new CBBNode1(*pS);

					pSNew->m_cotaInferior = pS->m_cotaInferior - v->m_minElement + taula[pV->iter][v->iter];

					if (pSNew->m_cotaInferior < cota_superiorGlobal + 1) //+1 per evitar errors de precisio
					{
						pSNew->m_Candidats.remove(v);

						pSNew->m_v = v;

						pSNew->m_CBBNode = pS;

						pSNew->m_cotaSuperior = pS->m_cotaSuperior - v->m_maxElement + taula[pV->iter][v->iter];
						if (pSNew->m_cotaSuperior < cota_superiorGlobal)
							cota_superiorGlobal = pSNew->m_cotaSuperior;

						queue.push(pSNew);
						SolutionNodesCreated++;
					}

				}
			}
			else
			{
				CBBNode1* pSNew2 = new CBBNode1(*pS);
				double w = pS->m_cotaInferior - pDesti->m_minElement + taula[pS->m_v->iter][pDesti->iter];
				if (w < cota_superiorGlobal + 1) //+1 per evitar errors de precisio
				{

					pSNew2->m_CBBNode = pS;
					pSNew2->m_v = pDesti;

					pSNew2->m_cotaInferior = w;

					pSNew2->m_cotaSuperior = pS->m_cotaSuperior - pDesti->m_maxElement + taula[pS->m_v->iter][pDesti->iter];
					if (pSNew2->m_cotaSuperior < cota_superiorGlobal)
						cota_superiorGlobal = pSNew2->m_cotaSuperior;

					queue.push(pSNew2);
					SolutionNodesCreated++;
				}
			}

		}
	}

	CTrack optimum(&g);
	optimum = construir_track(solucio, g);

	//cout << "NODES CREATED: " << SolutionNodesCreated << endl;

	while (!queue.empty()) {
		CBBNode1* pS = queue.top();
		queue.pop();
		pS->Unlink();
	}

	return optimum;
}
// SalesmanTrackBranchAndBound3 ===================================================

vector<vector<double>> construir_taula2(CGraph& g, CVisits& visits)
{
	vector<vector<double>> taula;
	for (CVertex* v : visits.m_Vertices) {
		Dijkstra(g, v);
		//DijkstraQueue(g, v);
		vector<double> taula_c;
		for (CVertex* aux : visits.m_Vertices) {
			taula_c.push_back(aux->m_DijkstraDistance);
		}
		taula.push_back(taula_c);
	}
	return taula;
}


CTrack SalesmanTrackBranchAndBound3(CGraph& g, CVisits& visits)
{
	SolutionNodesCreated = 0;
	int i = 0;
	for (CVertex* v0 : visits.m_Vertices)
	{
		v0->iter = i;
		i++;
	}
	visits.m_Vertices.front()->iter = 0;

	list<CVertex*> candidats_no_last = visits.m_Vertices;
	candidats_no_last.pop_back();

	vector<vector<double>> taula = construir_taula(g, visits);

	//mostrar_taula(taula);

	// CALCULEM LA COTA INICIAL, LA DEL NODE INICIAL
	list<CVertex*> candidats_aux = visits.m_Vertices;
	candidats_aux.pop_front();

	CVertex* pInici = visits.m_Vertices.front();
	CVertex* pDesti = visits.m_Vertices.back();

	priority_queue<CBBNode1*, std::vector<CBBNode1*>, comparator3> queue;
	candidats_aux.pop_back();

	CBBNode1* pSNew = new CBBNode1(&g, pInici, candidats_aux);
	pSNew->m_cotaInferior = 0;
	pSNew->m_Length = 0;

	vector<CVertex*> list_optimum;

	if (pSNew->m_Candidats.size() == 0)//simple
	{
		CBBNode1* pSDesti = new CBBNode1(&g, pDesti, candidats_aux);
		pSDesti->m_CBBNode = pSNew;
		pSDesti->m_cotaInferior = pSNew->m_cotaInferior + taula[pInici->iter][pDesti->iter];
		queue.push(pSDesti);
	}
	else
		queue.push(pSNew);

	SolutionNodesCreated++;
	CBBNode1* solucio = nullptr;

	double maxLength = numeric_limits<double>::max();
	while (!queue.empty()) {
		CBBNode1* pS = queue.top();
		queue.pop();

		CVertex* pV = pS->m_v;
		if (pV == pDesti && pS->m_Candidats.size() == 0) {
			// Solucion FINAL encontrada
			solucio = pS;
			break;
		}
		else {
			if (pS->m_Candidats.size() != 0)
			{
				for (CVertex* v : pS->m_Candidats) {
					double w = pS->m_Length + taula[pV->iter][v->iter] + v->m_Point.Distance(pDesti->m_Point);
					if (w < maxLength)
					{
						CBBNode1* pSNew = new CBBNode1(*pS);
						pSNew->m_Length = pS->m_Length + taula[pV->iter][v->iter];
						pSNew->m_cotaInferior = w;
						pSNew->m_Candidats.remove(v);
						pSNew->m_v = v;
						pSNew->m_CBBNode = pS;
						queue.push(pSNew);
						SolutionNodesCreated++;
					}
				}
			}
			else
			{
				double w = pS->m_Length + taula[pS->m_v->iter][pDesti->iter];
				if (w < maxLength)
				{
					CBBNode1* pSNew2 = new CBBNode1(*pS);
					pSNew2->m_CBBNode = pS;
					pSNew2->m_Length = w;
					pSNew2->m_v = pDesti;
					pSNew2->m_cotaInferior = w;
					queue.push(pSNew2);
					SolutionNodesCreated++;
					maxLength = pSNew2->m_cotaInferior;
				}
			}

		}
	}

	CTrack optimum(&g);
	optimum = construir_track(solucio, g);



	//cout << "NODES CREATED: " << SolutionNodesCreated << endl;

	while (!queue.empty()) {
		CBBNode1* pS = queue.top();
		queue.pop();
		pS->Unlink();
	}

	return optimum;
}
