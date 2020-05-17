/* Covid 19 Data update
 *  Country - India
 *  Confirmed, Recovered, Death and Test
 *  L.Boaz
 *  laxsam89@gmail.com
 *  whatsapp - 9597291816
 *  https://www.facebook.com/BoazHoney 
 *  https://tinyurl.com/MrBeeHoneyCoronaDashboard */
#include <ESP8266HTTPClient.h>
#include "json_parser.h"
#include "WifiConnect.h"
#include <ThingerESP8266.h>

#define USERNAME "thinger USERNAME"
#define DEVICE_ID "Your Device Name"
#define DEVICE_CREDENTIAL "Device password from thinger"
#define s2ms(second) (second*1000)
unsigned long long prev_millis(0);

#define country_code "Your Country Name"
#define SSID "Your Network Name"
#define SSID_PASSWORD "Your Network Password"

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

int interval = s2ms(5);
int confirmed, todayCases, total_deaths, todayDeaths, deaths, recovered, currentcase, critical, casesPerOneMillion, deathsPerOneMillion, totalTests, testsPerOneMillion;
unsigned long long PreviousMillis = 0;
unsigned long long CurrentMillis = interval;
bool bFirstKickMillis = false;

extern bool bGotIpFlag;

static String build_url_from_country(String country)
{
  String url = "http://coronavirus-19-api.herokuapp.com/countries/";
  url = url + country;
  return url;
}

void setup(void)
{
#if defined JSON_DEBUG
  Serial.begin(115200);
#endif
  JSON_LOG("Connecting...");
  vConnWifiNetworkViaSdk();
  Serial.println("https://www.worldometers.info/coronavirus/");
  thing.add_wifi(SSID, SSID_PASSWORD);

  thing["India Covid19"] >> [](pson & out)
  {
    out["confirmed"] = confirmed;
    out["Today Cases"] = todayCases;
    out["Cases/1 Million"] = casesPerOneMillion;
    out["Recovered"] = recovered;
    out["In Treatement"] = currentcase;
    out["Total Tests"] = totalTests;
    out["Tests/1 Million"] = testsPerOneMillion;
    out["In Critical"] = critical;
    out["Total Deaths"] = total_deaths;
    out["Today Deaths"] = todayDeaths;
    out["Deaths/1 Million"] = deathsPerOneMillion;
  };
}

void loop()
{
  thing.handle();
  if (bGotIpFlag) bGotIp();
  if (bFirstKickMillis) CurrentMillis = millis();
  if (!bGotIpFlag && CurrentMillis - PreviousMillis >= interval)
  {
    if (!bFirstKickMillis) CurrentMillis = 0;
    bFirstKickMillis = true;
    PreviousMillis = CurrentMillis;
    HTTPClient http;
    http.begin(build_url_from_country(country_code));
    int httpCode = http.GET();
    if (httpCode > 0)
    {
      String payload = http.getString();
      char* JsonArray = (char *)malloc(payload.length() + 1);
      if (!JsonArray) JSON_LOG("stuck");
      payload.toCharArray(JsonArray, payload.length() + 1);
      JSON_LOG(JsonArray);
      if (json_validate(JsonArray))
      {
        confirmed = (int)get_json_value(JsonArray, "cases", INT);
        todayCases = (int)get_json_value(JsonArray, "todayCases", INT);
        total_deaths = (int)get_json_value(JsonArray, "deaths", INT);
        todayDeaths = (int)get_json_value(JsonArray, "todayDeaths", INT);
        //deaths = (int)get_json_value(JsonArray, "deaths", INT);
        recovered = (int)get_json_value(JsonArray, "recovered", INT);
        currentcase = (int)get_json_value(JsonArray, "active", INT);
        critical = (int)get_json_value(JsonArray, "critical", INT);
        casesPerOneMillion = (int)get_json_value(JsonArray, "casesPerOneMillion", INT);
        deathsPerOneMillion = (int)get_json_value(JsonArray, "deathsPerOneMillion", INT);
        totalTests = (int)get_json_value(JsonArray, "totalTests", INT);
        testsPerOneMillion = (int)get_json_value(JsonArray, "testsPerOneMillion", INT);
      }
      free(JsonArray);
    }
    http.end();
  }
}
