#include "dcf77_meteotime.h"

const char mStringArrForecast[17][32] = {
	"--",
	"klar",
	"leicht bewölkt",
	"vorwiegend bewölkt",
	"bedeckt",
	"Hochnebel",
	"Nebel",
	"Regenschauer",
	"leichter Regen",
	"starker Regen",
	"Frontengewitter",
	"Wärmegewitter",
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
	"Fön",
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
	"Böen (Tag)",
	"Böen (Nacht)",
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
	"F - Bordeaux, Aquitaine (Südwestfrankreich)",
	"F - La Rochelle, Poitou-Charentes (Westküste Frankreichs)",
	"F - Paris, Île-de-France (Pariser Becken)",
	"F - Brest, Bretagne",
	"F - Clermont-Ferrand (Massif Central), Auvergne (Zentralmassiv)",
	"F - Béziers, Languedoc-Roussillon",
	"B - Bruxelles, Brussel (Benelux)",
	"F - Dijon (Bourgogne), Bourgogne (Ostfrankreich / Burgund)",
	"F - Marseille, Provence-Alpes-Côte d'Azur",
	"F - Lyon (Rhône-Alpes), Rhône-Alpes (Rhonetal)",
	"F - Grenoble (Savoie), Rhône-Alpes (Franz. Alpen)",
	"CH - La Chaux de Fond, Jura",
	"D - Frankfurt am Main, Hessen (Unterer Rheingraben)",
	"D - Trier, Westliches Mittelgebirge",
	"D - Duisburg, Nordrhein-Westfalen",
	"GB - Swansea, Wales (Westl. England / Wales)",
	"GB - Manchester, England (Nördliches England)",
	"F - le Havre, Haute-Normandie (Normandie)",
	"GB - London, England (Südostengland / London)",
	"D - Bremerhaven, Bremen (Nordseeküste)",
	"DK - Herning, Ringkobing (Nordwestliches Jütland)",
	"DK - Århus, Arhus (östliches Jütland)",
	"D - Hannover, Niedersachsen (Norddeutschland)",
	"DK - København, Staden Kobenhaven (Seeland)",
	"D - Rostock, Mecklenburg-Vorpommern (Ostseeküste)",
	"D - Ingolstadt, Bayern (Donautal)",
	"D - München, Bayern (Südbayern)",
	"I - Bolzano, Trentino-Alto Adige (Südtirol)",
	"D - Nürnberg, Bayern (Nordbayern)",
	"D - Leipzig, Sachsen",
	"D - Erfurt, Thüringen",
	"CH - Lausanne, Genferseeregion (Westl. Schweizer Mitteland)",
	"CH - Zürich (Östl. Schweizer Mittelland)",
	"CH - Adelboden (Westl. Schweizer Alpennordhang)",
	"CH - Sion, Wallis",
	"CH - Glarus, Östlicher Schweizer Alpennordhang",
	"CH - Davos, Graubünden",
	"D - Kassel, Hessen (Mittelgebirge Ost)",
	"CH - Locarno, Tessin",
	"I - Sestriere, Piemont. Alpen",
	"I - Milano, Lombardia (Poebene)",
	"I - Roma, Lazio (Toskana)",
	"NL - Amsterdam, Noord-Holland (Holland)",
	"I - Génova, Liguria (Golf von Genua)",
	"I - Venézia, Veneto (Pomündung)",
	"F - Strasbourg, Alsace (Oberer Rheingraben)",
	"A - Klagenfurt, Kärnten (Österreich. Alpensüdhang)",
	"A - Innsbruck, Tirol (Inneralpine Gebiete Österreichs)",
	"A - Salzburg, Bayr. / Österreich. Alpennordhang",
	"SK (Österreich / Slovakia) - Wien / Bratislava",
	"CZ - Praha, Prag (Tschechisches Becken)",
	"CZ - Decin, Severocesky (Erzgebirge)",
	"D - Berlin, Ostdeutschland",
	"S - Göteborg, Göteborgs och Bohus Län (Westküste Schweden)",
	"S - Stockholm, Stockholms Län (Stockholm)",
	"S - Kalmar, Kalmar Län (Schwedische Ostseeküste)",
	"S - Jönköping, Jönköpings Län (Südschweden)",
	"D - Donaueschingen, Baden-Württemberg (Schwarzwald / Schwäbische Alb)",
	"N - Oslo",
	"D - Stuttgart, Baden-Württemberg (Nördl. Baden Württemberg)",
	"I - Nápoli",
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
	"E - Gijón",
	"IRL - Galway",
	"IRL - Dublin",
	"GB - Glasgow",
	"N - Stavanger",
	"N - Trondheim",
	"S - Sundsvall",
	"PL - Gdansk",
	"PL - Warszawa",
	"PL - Kraków",
	"S - Umea",
	"S - Oestersund",
	"CH - Samedan",
	"CR - Zagreb",
	"CH - Zermatt",
	"CR - Split"
};

const char mStringArrForecastType60[8][48] = {
	"(Höchstwerte, 1. Tag (Heute))",
	"(Tiefstwerte, 1. Tag (Heute))",
	"(Höchstwerte, 2. Tag)",
	"(Tiefstwerte, 2. Tag)",
	"(Höchstwerte, 3. Tag)",
	"(Tiefstwerte, 3. Tag)",
	"(Höchstwerte, 4. Tag)",
	"(Nur Winddaten und Wetteranomalien, 4. Tag)"
};

const char mStringArrForecastType90[2][64] = {
	"(Nur Wetterprognose und Temperatur, 1. Tag (Heute))",
	"(Nur Wetterprognose und Temperatur, 2. Tag)"
};

const char mStringArrDecoderStatus[4][64] = {
	"Hinweis: Dekodierung fehlgeschlagen!\r\n",
	"",
	"Hinweis: Decoder hat unbekannten Status zurückgegeben.\r\n",
	"Hinweis: Es wurde eine Datennkorrektur durchgeführt!\r\n"
};
