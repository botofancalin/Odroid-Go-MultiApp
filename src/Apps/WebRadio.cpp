#include "WebRadio.h"

void WebRadioClass::getvolume()
{
	preferences.begin("Volume", false);
	GO.vol = preferences.getFloat("vol", 15.0f);
	preferences.end();
}

void WebRadioClass::setVolume(int *v)
{
	float volume = *v / 195.7f; // volme max value can be 3.99
	out->SetGain(volume);
}

void WebRadioClass::StopPlaying()
{
	if (player)
	{
		player->stop();
		delete player;
		player = NULL;
	}
	if (buff)
	{
		buff->close();
		delete buff;
		buff = NULL;
	}
	if (file)
	{
		file->close();
		delete file;
		file = NULL;
	}
}

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
String _s2, _s3;
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
	String s1 = string;
	String s2 = strstr(s1.c_str(), "/");
	String s3 = s1;
	s3.replace(s2, "");
	s2.replace("/", "");
	GO.Lcd.setTextColor(BLACK);
	GO.Lcd.setFreeFont(MDFONT);
	GO.Lcd.drawCentreString(_s3, 160, 80, 1);
	GO.Lcd.drawCentreString(_s2, 160, 105, 1);
	GO.Lcd.setTextColor(ORANGE);
	GO.Lcd.drawCentreString(s3, 160, 80, 1);
	GO.Lcd.drawCentreString(s2, 160, 105, 1);
	_s2 = s2;
	_s3 = s3;
}

bool WebRadioClass::GetStations(fs::FS &fs, const char *path)
{
	File sdfile = fs.open(path);
	if (!sdfile)
	{
		return false;
	}

	if (sdfile.available())
	{
		StationList = sdfile.readStringUntil(EOF);
		while (StationList != "\0")
		{
			int t = StationList.indexOf("=");
			name = StationList.substring(0, t);
			StationList.replace(String(name + "="), "");
			int r = StationList.indexOf(";");
			link = StationList.substring(0, r);
			StationList.replace(String(link + ";"), "");
			if (StationList != "\0")
			{
				StationList = StationList.substring(2);
			}
			Name.push_back(name);
			Link.push_back(link);
		}
	}
	sdfile.close();
	return true;
}

void WebRadioClass::Run()
{
	GO.update();
	getvolume();
	GO.drawAppMenu(F("WebRadio"), F("Vol-"), F("Next"), F("Vol+"));
	GO.Lcd.setTextColor(ORANGE);
	GO.Lcd.drawCentreString("Press B to Exit", 158, 190, 2);
	preallocateBuffer = malloc(preallocateBufferSize);
	preallocateCodec = malloc(preallocateCodecSize);
	out = new AudioOutputI2S(0, 1);
	out->SetOutputModeMono(true);

	if (GetStations(My_SD, "/RadioStations.txt"))
	{
		if (WiFi.isConnected())
		{

			while (play)
			{
				unsigned long now = millis();
				if (now - lastcheck >= 1000)
				{
					GO.Lcd.setTextColor(WHITE, 15);
					GO.Lcd.drawString("Batt: " + String(GO.battery.getPercentage()) + " %", 10, 5, 2);
					SignalStrength = map(100 + WiFi.RSSI(), 5, 90, 0, 100);
					GO.Lcd.drawRightString("WiFi: " + String(SignalStrength) + " %", 310, 5, 2);
					lastcheck = now;
				}
				if ((GO.JOY_Y.wasAxisPressed() == 1) && GO.vol > 0)
				{
					GO.vol -= 5;
					setVolume(&GO.vol);
				}
				if ((GO.JOY_Y.wasAxisPressed() == 2) && GO.vol < 100)
				{
					GO.vol += 5;
					setVolume(&GO.vol);
				}

				if ((GO.JOY_X.wasAxisPressed() == 1))
				{
					StopPlaying();
					if (Station > 0)
					{
						Station--;
					}
					else
					{
						Station = (unsigned int)(Link.size() - 1);
					}
					upd = true;
				}
				if ((GO.JOY_X.wasAxisPressed() == 2))
				{
					StopPlaying();
					if (Station < (unsigned int)(Link.size() - 1))
					{
						Station++;
					}
					else
					{
						Station = 0;
					}
					upd = true;
				}
				if (GO.BtnB.wasPressed())
				{
					play = false;
				}
				if (GO.vol != GO.old_vol)
				{
					GO.Lcd.HprogressBar(80, 170, 200, 15, GREEN, GO.vol, true);
					GO.old_vol = GO.vol;
				}
				GO.update();

				if (upd)
				{
					GO.Lcd.setTextColor(RED);
					GO.Lcd.drawString("Buffer %", 15, 148, 2);
					GO.Lcd.HprogressBar(80, 150, 200, 15, RED, 0, true);
					rawFillLvl = 0;
					GO.Lcd.setTextColor(GREEN);
					GO.Lcd.drawString("Volume", 15, 168, 2);
					GO.Lcd.HprogressBar(80, 170, 200, 15, GREEN, GO.vol, true);
					GO.Lcd.setFreeFont(STFONT);
					GO.Lcd.setTextColor(BLACK);
					GO.Lcd.drawCentreString(old_Station, 160, 35, 1);
					GO.Lcd.setTextColor(PINK);
					GO.Lcd.drawCentreString(Name[Station], 160, 35, 1);
					old_Station = Name[Station];
					file = new AudioFileSourceICYStream(Link[Station].c_str());
					file->RegisterMetadataCB(MDCallback, (void *)"ICY");
					buff = new AudioFileSourceBuffer(file, preallocateBuffer, preallocateBufferSize);
					player = new AudioGeneratorMP3(preallocateCodec, preallocateCodecSize);
					player->begin(buff, out);
					setVolume(&GO.vol);
					GO.old_vol = GO.vol;
					upd = false;
				}
				else
				{
					player->loop();
					if (rawFillLvl != buff->getFillLevel())
					{
						rawFillLvl = buff->getFillLevel();
						fillLvl = map(rawFillLvl, 0, preallocateBufferSize, 0, 100);
						GO.Lcd.HprogressBar(80, 150, 200, 15, RED, fillLvl, true);
					}
					if (!upd && !fillLvl)
					{
						StopPlaying();
						if (Station < (unsigned int)(Link.size() - 1))
						{
							Station++;
							upd = true;
						}
						else
						{
							break;
						}
					}
				}
			}
			preferences.begin("Volume", false);
			preferences.putFloat("vol", GO.vol);
			preferences.end();
			StopPlaying();
			if (out)
			{
				out->stop();
				delete out;
				out = NULL;
			}
			free(preallocateBuffer);
			free(preallocateCodec);
			dacWrite(25, 0);
			dacWrite(26, 0);
		}
		else
		{
			GO.Lcd.setTextColor(WHITE);
			GO.Lcd.drawCentreString("Wifi Not Connected!", 160, 60, 2);
			delay(3000);
		}
	}
	else
	{
		GO.Lcd.setTextColor(WHITE);
		GO.Lcd.drawCentreString("RadioStations.txt Not Found on SD", 160, 60, 2);
		delay(3000);
	}
}

WebRadioClass::WebRadioClass()
{
}

WebRadioClass::~WebRadioClass()
{
	Name.clear();
	Name.shrink_to_fit();
	Link.clear();
	Link.shrink_to_fit();
	GO.Lcd.fillScreen(0);
	GO.Lcd.setTextSize(1);
	GO.Lcd.setTextFont(1);
	GO.show();
}