#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <sys/time.h>                   // struct timeval
#include <coredecls.h>                  // settimeofday_cb()
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include <time.h>

const char* ssid       = "Nhatrang Hitech";
const char* password   = "nhtc2109";

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 3600 * 7;
const int   daylightOffset_sec = 3600;

/* cài đặt thời gian */
void Settime(void) {
  struct tm desired_tm;
  time_t false_now = 0;
  memset(&desired_tm, 0, sizeof(struct tm));
  // Setting up a false actual time - used when the file is created and for modification with the current time
  desired_tm.tm_mon = 10 - 1;
  desired_tm.tm_mday = 31;
  desired_tm.tm_year = 2018 - 1900;
  desired_tm.tm_hour = 10;
  desired_tm.tm_min = 35;
  desired_tm.tm_sec = 23;
  false_now = mktime(&desired_tm);
  struct timeval now = { .tv_sec = false_now };
  settimeofday(&now, NULL);
}

#ifdef ESP8266
bool getLocalTime(struct tm * info, uint32_t ms)
{
    uint32_t count = ms / 10;
    time_t now;

    time(&now);
    localtime_r(&now, info);

    if(info->tm_year > (2016 - 1900)){
        return true;
    }

    while(count--) {
        delay(10);
        time(&now);
        localtime_r(&now, info);
        if(info->tm_year > (2016 - 1900)){
            return true;
        }
    }
    return false;
}
#endif

/* đọc thời gian */
void printLocalTime()
{
  struct tm tmstruct;
  if (!getLocalTime(&tmstruct,5000)) {
    Serial.println("Failed to obtain time");
    return;
  }
  #ifdef ESP32
  Serial.println(&tmstruct, "%A, %B %d %Y %H:%M:%S"); //http://www.cplusplus.com/reference/ctime/strftime/
  #elif defined(ESP8266)
  char buf[64];
  size_t written = strftime(buf, 64, "%A, %B %d %Y %H:%M:%S", &tmstruct);
  Serial.println(buf);
  //Serial.printf("  LAST READ: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
  #endif
  struct timeval now;
  gettimeofday(&now, NULL);
  // or
  //now.tv_sec = time(nullptr);
  localtime_r(&now.tv_sec, &tmstruct);
  #ifdef ESP32
  Serial.println(&tmstruct, "%A, %B %d %Y %H:%M:%S"); //http://www.cplusplus.com/reference/ctime/strftime/
  #elif defined(ESP8266)
  size_t written = strftime(buf, 64, "%A, %B %d %Y %H:%M:%S", &tmstruct);
  Serial.println(buf);
  //Serial.printf("  LAST READ: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
  #endif
}

void setup()
{
  Serial.begin(115200);
  /*test cài đặt thời gian 
  Nếu cài đặt thời gian tại đây. Nhưng sau đó chạy NTP thì thời gian sẽ được cập nhật lại
  theo giờ internet*/
  Settime();   
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  printLocalTime();

  /* disconnect WiFi as it's no longer needed
  Khi NTP cập nhật thời gian OK thì ko cần kết internet vẫn chạy rtc offline được */
  //WiFi.disconnect(true);
  //WiFi.mode(WIFI_OFF);
}

void loop()
{
  delay(1000);
  printLocalTime();
}
