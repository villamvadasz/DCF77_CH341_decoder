#include "dcf77_meteotime.h"

const char mStringArrForecast[17][32] = {
	"--",
	"klar",
	"leicht bew�lkt",
	"vorwiegend bew�lkt",
	"bedeckt",
	"Hochnebel",
	"Nebel",
	"Regenschauer",
	"leichter Regen",
	"starker Regen",
	"Frontengewitter",
	"W�rmegewitter",
	"Schneeregenschauer",
	"Schneeschauer",
	"Schneeregen",
	"Schneefall",
	"sonnig"
};

const char mByteArrForecastLookup[17] = {
	0,
	1,
	2,
	3,
	4,
	11,
	9,
	15,
	6,
	14,
	7,
	8,
	13,
	10,
	5,
	12,
	16
};

const char mStringArrWind[16][18] = {
	"N",
	"NO",
	"O",
	"SO",
	"S",
	"SW",
	"W",
	"NW",
	"wechselnd",
	"F�n",
	"Bise N/O",
	"Mistral N",
	"Scirocco S",
	"Tramontana W",
	"--",
	"--"
};

const char mStringArrWindStrength[8][16] = {
	"0",
	"0-2",
	"3-4",
	"5-6",
	"7",
	"8",
	"9",
	">=10"
};

const char mStringArrExtremeWeather[16][32] = {
	"Keine",
	"Schweres Wetter (24h)",
	"Schweres Wetter (Tag)",
	"Schweres Wetter (Nacht)",
	"Sturm",
	"Sturm (Tag)",
	"Sturm (Nacht)",
	"B�en (Tag)",
	"B�en (Nacht)",
	"Eisregen (Vormittag)",
	"Eisregen (Nachmittag)",
	"Eisregen (Nacht)",
	"Feinstaub",
	"Ozon",
	"Radiation",
	"Hochwasser"
};

const char mStringArrPrecipitation[8][8] = {
	"0%",
	"15%",
	"30%",
	"45%",
	"60%",
	"75%",
	"90%",
	"100%"
};

const char mStringArrRelative[4][16] = {
	"Gleichbleibend",
	"Sprung (1)",
	"Sprung (2)",
	"Sprung (3)"
};

const char mStringArrSunshine[4][16] = {
	"0-2 Stunden",
	"2-4 Stunden",
	"5-6 Stunden",
	"7-8 Stunden"
};

const char mStringArrRegions[90][72] = {
	"F - Bordeaux, Aquitaine (S�dwestfrankreich)",
	"F - La Rochelle, Poitou-Charentes (Westk�ste Frankreichs)",
	"F - Paris, �le-de-France (Pariser Becken)",
	"F - Brest, Bretagne",
	"F - Clermont-Ferrand (Massif Central), Auvergne (Zentralmassiv)",
	"F - B�ziers, Languedoc-Roussillon",
	"B - Bruxelles, Brussel (Benelux)",
	"F - Dijon (Bourgogne), Bourgogne (Ostfrankreich / Burgund)",
	"F - Marseille, Provence-Alpes-C�te d'Azur",
	"F - Lyon (Rh�ne-Alpes), Rh�ne-Alpes (Rhonetal)",
	"F - Grenoble (Savoie), Rh�ne-Alpes (Franz. Alpen)",
	"CH - La Chaux de Fond, Jura",
	"D - Frankfurt am Main, Hessen (Unterer Rheingraben)",
	"D - Trier, Westliches Mittelgebirge",
	"D - Duisburg, Nordrhein-Westfalen",
	"GB - Swansea, Wales (Westl. England / Wales)",
	"GB - Manchester, England (N�rdliches England)",
	"F - le Havre, Haute-Normandie (Normandie)",
	"GB - London, England (S�dostengland / London)",
	"D - Bremerhaven, Bremen (Nordseek�ste)",
	"DK - Herning, Ringkobing (Nordwestliches J�tland)",
	"DK - �rhus, Arhus (�stliches J�tland)",
	"D - Hannover, Niedersachsen (Norddeutschland)",
	"DK - K�benhavn, Staden Kobenhaven (Seeland)",
	"D - Rostock, Mecklenburg-Vorpommern (Ostseek�ste)",
	"D - Ingolstadt, Bayern (Donautal)",
	"D - M�nchen, Bayern (S�dbayern)",
	"I - Bolzano, Trentino-Alto Adige (S�dtirol)",
	"D - N�rnberg, Bayern (Nordbayern)",
	"D - Leipzig, Sachsen",
	"D - Erfurt, Th�ringen",
	"CH - Lausanne, Genferseeregion (Westl. Schweizer Mitteland)",
	"CH - Z�rich (�stl. Schweizer Mittelland)",
	"CH - Adelboden (Westl. Schweizer Alpennordhang)",
	"CH - Sion, Wallis",
	"CH - Glarus, �stlicher Schweizer Alpennordhang",
	"CH - Davos, Graub�nden",
	"D - Kassel, Hessen (Mittelgebirge Ost)",
	"CH - Locarno, Tessin",
	"I - Sestriere, Piemont. Alpen",
	"I - Milano, Lombardia (Poebene)",
	"I - Roma, Lazio (Toskana)",
	"NL - Amsterdam, Noord-Holland (Holland)",
	"I - G�nova, Liguria (Golf von Genua)",
	"I - Ven�zia, Veneto (Pom�ndung)",
	"F - Strasbourg, Alsace (Oberer Rheingraben)",
	"A - Klagenfurt, K�rnten (�sterreich. Alpens�dhang)",
	"A - Innsbruck, Tirol (Inneralpine Gebiete �sterreichs)",
	"A - Salzburg, Bayr. / �sterreich. Alpennordhang",
	"SK (�sterreich / Slovakia) - Wien / Bratislava",
	"CZ - Praha, Prag (Tschechisches Becken)",
	"CZ - Decin, Severocesky (Erzgebirge)",
	"D - Berlin, Ostdeutschland",
	"S - G�teborg, G�teborgs och Bohus L�n (Westk�ste Schweden)",
	"S - Stockholm, Stockholms L�n (Stockholm)",
	"S - Kalmar, Kalmar L�n (Schwedische Ostseek�ste)",
	"S - J�nk�ping, J�nk�pings L�n (S�dschweden)",
	"D - Donaueschingen, Baden-W�rttemberg (Schwarzwald / Schw�bische Alb)",
	"N - Oslo",
	"D - Stuttgart, Baden-W�rttemberg (N�rdl. Baden W�rttemberg)",
	"I - N�poli",
	"I - Ancona",
	"I - Bari",
	"HU - Budapest",
	"E - Madrid",
	"E - Bilbao",
	"I - Palermo",
	"E - Palma de Mallorca",
	"E - Valencia",
	"E - Barcelona",
	"AND - Andorra",
	"E - Sevilla",
	"P - Lissabon",
	"I - Sassari, (Sardinien / Korsika)",
	"E - Gij�n",
	"IRL - Galway",
	"IRL - Dublin",
	"GB - Glasgow",
	"N - Stavanger",
	"N - Trondheim",
	"S - Sundsvall",
	"PL - Gdansk",
	"PL - Warszawa",
	"PL - Krak�w",
	"S - Umea",
	"S - Oestersund",
	"CH - Samedan",
	"CR - Zagreb",
	"CH - Zermatt",
	"CR - Split"
};

const char mStringArrForecastType60[8][48] = {
	"(H�chstwerte, 1. Tag (Heute))",
	"(Tiefstwerte, 1. Tag (Heute))",
	"(H�chstwerte, 2. Tag)",
	"(Tiefstwerte, 2. Tag)",
	"(H�chstwerte, 3. Tag)",
	"(Tiefstwerte, 3. Tag)",
	"(H�chstwerte, 4. Tag)",
	"(Nur Winddaten und Wetteranomalien, 4. Tag)"
};

const char mStringArrForecastType90[2][64] = {
	"(Nur Wetterprognose und Temperatur, 1. Tag (Heute))",
	"(Nur Wetterprognose und Temperatur, 2. Tag)"
};

const char mStringArrDecoderStatus[4][64] = {
	"Hinweis: Dekodierung fehlgeschlagen!\r\n",
	"",
	"Hinweis: Decoder hat unbekannten Status zur�ckgegeben.\r\n",
	"Hinweis: Es wurde eine Datennkorrektur durchgef�hrt!\r\n"
};
