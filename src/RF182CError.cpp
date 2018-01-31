
#include "RF182CError.h"

RF182CErrorRef RF182CError::create(string errorCode)
{
	return RF182CErrorRef(new RF182CError(errorCode))->shared_from_this();
}

RF182CError::RF182CError(string errorCode) : mCode(errorCode)
{
	stringstream stream;
	stream << std::hex << mCode;
	stream >> mCodeInt;

	mWhat = parse();
}

string RF182CError::parse()
{
	if (mCode == "0000")
	{
		return "Kein Fehler";
	}
	else if (mCode == "0001")
	{
		return "Anwesenheitsfehler: Der MDS ist aus dem Übertragungsfenster des SLG gefahren. Der MOBY - Befehl wurde nur teilweise abgearbeitet. Lesebefehl: Es werden keine Daten an den Client geliefert. Schreibbefehl : Der MDS, der gerade das Feld verlassen hat, beinhaltet einen unvollständigen Datensatz.";
	}
	else if (mCode == "0002")
	{
		return "Anwesenheitsfehler: Ein MDS ist an einem SLG vorbeigefahren und wurde mit keinem MOBY - Befehl bearbeitet.";
	}
	else if (mCode == "0003")
	{
		return "Fehler in der Verbindung zum SLG; das SLG antwortet nicht.";
	}
	else if (mCode == "0004")
	{
		return "Fehler im Speicher des MDS - Der MDS wurde noch nie beschrieben oder hat durch einen Ausfall der Batterie seinen	Speicherinhalt verloren";
	}
	else if (mCode == "0005")
	{
		return "unbekannter Befehl - Der Client gibt einen nicht interpretierbaren Befehl an das Kommunikationsmodul.";
	}
	else if (mCode == "0006")
	{
		return "Feldstörung am SLG - Das SLG empfängt Störimpulse aus der Umgebung.";
	}
	else if (mCode == "0007")
	{
		return "zu viele Sendefehler Der MDS konnte den Befehl oder die Schreibdaten vom Kommunikationsmodul nach mehreren Versuchen nicht richtig empfangen.";
	}
	else if (mCode == "0008")
	{
		return "CRC-Sendefehler";
	}
	else if (mCode == "0009")
	{
		return "Nur bei Initialisierung: CRC-Fehler beim Quittungsempfang vom MDS - Ursache wie bei Fehler 0006";
	}
	else if (mCode == "000A")
	{
		return "Nur bei Initialisierung: MDS kann den Initialisierungs-Befehl nicht durchführen - MDS ist defekt";
	}
	else if (mCode == "000B")
	{
		return "MOBY U: Speicher des MDS ist nicht korrekt lesbar";
	}
	else if (mCode == "000C")
	{
		return "Speicher des MDS kann nicht beschrieben werden";
	}
	else if (mCode == "000D")
	{
		return "Adressfehler - Der Adressbereich des MDS wird überschritten.";
	}
	else if (mCode == "000E")
	{
		return "ECC-Fehler (nur möglich, wenn ECC_mode = TRUE) Die Daten können nicht vom MDS gelesen werden.";
	}
	else if (mCode == "000F")
	{
		return "Hochlaufmeldung eines am Kommunikationsmodul angeschlossenen Readers";
	}
	else if (mCode == "0011")
	{
		return "Kurzschluss oder Überlastung der 24 V-Ausgänge (Fehlercode, Anwesenheit)";
	}
	else if (mCode == "0012")
	{
		return "Interner Kommunikationsmodul-Kommunikationsfehler.";
	}
	else if (mCode == "0013")
	{
		return "Es sind nicht genügend Puffer im Kommunikationsmodul/SLG U für die Zwischenspeicherung des Befehls vorhanden.";
	}
	else if (mCode == "0014")
	{
		return "Kommunikationsmodul/SLG-interner Fehler.";
	}
	else if (mCode == "0015")
	{
		return "Fehlerhafte Parametrierung des Kommunikationsmodul/SLG";
	}
	else if (mCode == "0016")
	{
		return "Das Kommunikationsmodul kann den Befehl nicht bearbeiten.";
	}
	else if (mCode == "0017")
	{
		return "Kommunikationsfehler";
	}
	else if (mCode == "0018")
	{
		return "Es ist ein Fehler aufgetreten, der mit einem RESET quittiert werden muss.";
	}
	else if (mCode == "0019")
	{
		return "Vorheriger Befehl ist aktiv bzw. Pufferüberlauf";
	}
	else if (mCode == "001C")
	{
		return "Die Antenne am SLG ist abgeschaltet. In diesem Zustand wurde ein MDS-Befehl zum Kommunikationsmodul gestartet.";
	}
	else if (mCode == "001D")
	{
		return "Es sind mehr MDS im Übertragungsfenster, als das SLG gleichzeitig bearbeiten kann.";
	}
	else if (mCode == "001E")
	{
		return "Fehler beim Bearbeiten der Funktion";
	}
	else if (mCode == "001F")
	{
		return "Laufender Befehl durch RESET abgebrochen";
	}
	else if (mCode == "3214")
	{
		return "Fatal Error. Interner Firmwarefehler des Kommunikationsmodules";
	}
	else if (mCode == "3221")
	{
		return "Konfigurierung erforderlich";
	}
	else if (mCode == "3222")
	{
		return "Konfigurierungskonflikt des Kommunikationsmoduls.";
	}
	else if (mCode == "3223")
	{
		return "Konfigurierung fehlerhaft";
	}
	else if (mCode == "3321")
	{
		return "Interner Verarbeitungsfehler";
	}
	else if (mCode == "3322")
	{
		return "Bufferüberlauf im Kommunikationsmodul";
	}
	else if (mCode == "3323")
	{
		return "Bufferüberlauf des Notifikationbuffers";
	}
	else if (mCode == "3324")
	{
		return "Bufferüberlauf des Alarmbuffers";
	}
	else if (mCode == "3325")
	{
		return "Fehler in der internen Datenverarbeitung des Kommunikationsmoduls";
	}
	else if (mCode == "3326")
	{
		return "Fehler des internen Readerschnittstelle des Kommunikationsmoduls";
	}
	else if (mCode == "3417")
	{
		return "Längenfehler beim Empfang von Daten über die TCP/IP Verbindung.";
	}
	else if (mCode == "3421")
	{
		return "Fehler in der Konfiguration der Verbindung";
	}
	else if (mCode == "3422")
	{
		return "Fehler beim Senden von Daten über die TCP/IP Verbindung";
	}
	else if (mCode == "3423")
	{
		return "Timeout beim Empfang von Daten über die TCP/IP Verbindung.";
	}
	else
	{
		return "Unbekannter Fehler";
	}
}
