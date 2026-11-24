#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>    
#include <ctype.h>     
#include <time.h>      
void speak(const char *sentence)
{
    char command[600];
    printf("Gen: %s\n", sentence);

    sprintf(command,
        "powershell -Command \"Add-Type -AssemblyName System.Speech; "
        "(New-Object System.Speech.Synthesis.SpeechSynthesizer).Speak('%s')\"",
        sentence);

    system(command);
}

void listen(char *heard, int size)
{
    FILE *pipe;
    int i;

    heard[0] = '\0';   

    pipe = _popen(
        "powershell -Command \""
        "Add-Type -AssemblyName System.Speech; "
        "$r = New-Object System.Speech.Recognition.SpeechRecognitionEngine; "
        "$r.SetInputToDefaultAudioDevice(); "
        "$r.LoadGrammar((New-Object System.Speech.Recognition.DictationGrammar)); "
        "$x = $r.Recognize([TimeSpan]::FromSeconds(6)); "
        "if ($x) { $x.Text }\"",
        "r");

    if (pipe == NULL) {
        return;   
    }

    fgets(heard, size, pipe);
    _pclose(pipe);

    heard[strcspn(heard, "\r\n")] = '\0';
    for (i = 0; heard[i] != '\0'; i++) {
        heard[i] = tolower(heard[i]);
    }
}

int main(void)
{
    char heard[50];       
    char answer[500];      
    time_t now;            
    int keep_going = 1;    

    speak("Hello. I am Gen, your voice assistant. How can I help you?");

    while (keep_going == 1)
    {
        printf("\n[Listening... speak now]\n");
        listen(heard, sizeof(heard));

        if (strlen(heard) == 0) {
            printf("(I heard nothing)\n");
            continue;
        }

        printf("You said: %s\n", heard);


        if (strstr(heard, "hello") != NULL) {
            speak("Hello! I am Gen. Nice to meet you.");
        }
        else if (strstr(heard, "your name") != NULL || strstr(heard, "who are you") != NULL) {
            speak("My name is Gen. I am your voice assistant.");
        }
        else if (strstr(heard, "what is thetime") != NULL) {
            now = time(NULL);
            strftime(answer, sizeof(answer), "The time is %I:%M %p", localtime(&now));
            speak(answer);
        }
        else if (strstr(heard, "what is thedate") != NULL) {
            now = time(NULL);
            strftime(answer, sizeof(answer), "Today is %A, %d %B %Y", localtime(&now));
            speak(answer);
        }
        else if (strstr(heard, "opennotepad") != NULL) {
            speak("Opening Notepad.");
            system("start notepad");
        }
        else if (strstr(heard, "opencalculator") != NULL) {
            speak("Opening Calculator.");
            system("start calc");
        }
        else if (strstr(heard, "open google") != NULL) {
            speak("Opening Google.");
            system("start https://www.google.com");
        }
        else if (strstr(heard, "tell me ajoke") != NULL) {
            speak("Why do programmers like dark mode? Because light attracts bugs.");
        }
        else if (strstr(heard, "exit") != NULL || strstr(heard, "bye") != NULL) {
            speak("Goodbye! Gen signing off.");
            keep_going = 0;   
        }
        else {
            speak("Sorry, I did not understand that.");
        }
    }

    return 0;
}