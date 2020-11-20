#include <string>
#include <iostream>

using namespace std;

#include "search_server.h"
#include "request_queue.h"
#include "paginator.h"
#include "tests.h"

int main() {

    TestSearchServer();

    {
        SearchServer search_server("� � ��"s);

        AddDocument(search_server, 1, "�������� ��� �������� �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
        AddDocument(search_server, 1, "�������� �� � ������ �������"s, DocumentStatus::ACTUAL, { 1, 2 });
        AddDocument(search_server, -1, "�������� �� � ������ �������"s, DocumentStatus::ACTUAL, { 1, 2 });
        AddDocument(search_server, 3, "������� �� ����\x12��� �������"s, DocumentStatus::ACTUAL, { 1, 3, 2 });
        AddDocument(search_server, 4, "������� �� ������� �������"s, DocumentStatus::ACTUAL, { 1, 1, 1 });

        FindTopDocuments(search_server, "�������� -��"s);
        FindTopDocuments(search_server, "�������� --���"s);
        FindTopDocuments(search_server, "�������� -"s);

        MatchDocuments(search_server, "�������� ��"s);
        MatchDocuments(search_server, "������ -���"s);
        MatchDocuments(search_server, "������ --��"s);
        MatchDocuments(search_server, "�������� - �����"s);
    }

    {
        SearchServer search_server;
        search_server.SetStopWords("� � ��"s);

        search_server.AddDocument(0, "����� ��� � ������ �������"s, DocumentStatus::ACTUAL, { 8, -3 });
        search_server.AddDocument(1, "�������� ��� �������� �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
        search_server.AddDocument(2, "��������� �� ������������� �����"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
        search_server.AddDocument(3, "��������� ������� �������"s, DocumentStatus::BANNED, { 9 });

        cout << "ACTUAL by default:"s << endl;
        for (const Document& document : search_server.FindTopDocuments("�������� ��������� ���"s)) {
            PrintDocument(document);
        }

        cout << "BANNED:"s << endl;
        for (const Document& document : search_server.FindTopDocuments("�������� ��������� ���"s, DocumentStatus::BANNED)) {
            PrintDocument(document);
        }

        cout << "Even ids:"s << endl;
        for (const Document& document : search_server.FindTopDocuments("�������� ��������� ���"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; })) {
            PrintDocument(document);
        }
    }

    {
        SearchServer search_server("� � ��"s);
        RequestQueue request_queue(search_server);

        search_server.AddDocument(1, "�������� ��� �������� �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
        search_server.AddDocument(2, "�������� �� � ������ �������"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
        search_server.AddDocument(3, "������� ��� ������ ������� "s, DocumentStatus::ACTUAL, { 1, 2, 8 });
        search_server.AddDocument(4, "������� �� ������� �������"s, DocumentStatus::ACTUAL, { 1, 3, 2 });
        search_server.AddDocument(5, "������� �� ������� �������"s, DocumentStatus::ACTUAL, { 1, 1, 1 });

        // 1439 �������� � ������� �����������
        for (int i = 0; i < 1439; ++i) {
            request_queue.AddFindRequest("������ ������"s);
        }
        // ��� ��� 1439 �������� � ������� �����������
        request_queue.AddFindRequest("�������� ��"s);
        // ����� �����, ������ ������ ������, 1438 �������� � ������� �����������
        request_queue.AddFindRequest("������� �������"s);
        // ������ ������ ������, 1437 �������� � ������� �����������
        request_queue.AddFindRequest("�������"s);
        cout << "��������, �� ������� ������ �� ������� "s << request_queue.GetNoResultRequests();
    }
}
