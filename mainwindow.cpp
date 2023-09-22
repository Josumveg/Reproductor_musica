#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <Arduino.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int process_csv();

    // Create model
    model = new QStringListModel(this);

    //Make data
    QStringList List;
    List << "Hola" << "ay mi madre";

    //Populate model
    model->setStringList(List);

    //Put model and view together
    ui->listViewTracks->setModel(model);
    ui->listViewTracks->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listViewTracks->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Obtener el uso de recursos
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        // Mostrar información sobre el uso de memoria
        qDebug() << "Uso de memoria residente (en KB):" << usage.ru_maxrss;
        qDebug() << "Uso de memoria compartida (en KB):" << usage.ru_ixrss;
        qDebug() << "Tamaño máximo de pila (en KB):" << usage.ru_maxrss;
        qDebug() << "Tamaño máximo de datos (en KB):" << usage.ru_idrss;
    } else {
        qDebug() << "No se pudo obtener información sobre el uso de memoria.";

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_listViewTracks_clicked(const QModelIndex &index)
{
    QString selectedItem = index.data(Qt::DisplayRole).toString();
    qDebug() << selectedItem;
    std::string str = selectedItem.toStdString();
    std::cout << str << std::endl;
}

int process_csv() {
    int cont = 1;
    std::ifstream file("/home/jose/Downloads/fma_metadata/tracks.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Replace newline characters with spaces
        std::replace(line.begin(), line.end(), '\n', ' ');

        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string field;

        while (std::getline(ss, field, ',')) {
            fields.push_back(field);
        }

        // Now, 'fields' contains the correctly parsed fields, with newline characters replaced by spaces
        // You can process or store these fields as needed
        for (const std::string& value : fields) {
            std::cout << fields[0] << std::endl;
        }
        std::cout << std::endl;
    }

    file.close();
    return 0;
}

void loadSong()
{
    if (!music.openFromFile("ruta_de_la_cancion.ogg"))
    {
        qDebug() << "No se pudo abrir la canción.";
            return;
    }

    // Cambia el botón a "Reproducir" cuando se carga la canción
        playButton->setText("Reproducir");
    layout()->addWidget(playButton);
}

void playOrPause()
{
    if (music.getStatus() == sf::Music::Playing)
    {
        music.pause();
        playButton->setText("Continuar");
    }
    else if (music.getStatus() == sf::Music::Paused)
    {
        music.play();
        playButton->setText("Pausar");
    }
    else
    {
        qDebug() << "La canción no se ha cargado o está en un estado desconocido.";
        }
}

void setSongPosition(int value)
{
        qint64 position = player->duration() * value / 100;
        player->setPosition(position);
}

bool compareArtists(const Song& song1, const Song& song2) {
        // Función de comparación personalizada
    // Si el nombre del artista contiene números o caracteres no alfabéticos,
    // los colocamos después de 'z' en la ordenación.
    QString artist1 = song1.artist;
        QString artist2 = song2.artist;

        for (int i = 0; i < artist1.length(); ++i) {
        QChar c = artist1.at(i);
        if (!c.isLetter())
            artist1[i] = 'z' + 1;  // Colocar caracteres no alfabéticos después de 'z'
        else
            artist1[i] = c.toLower(); // Convertir a minúscula para comparación sin distinción entre mayúsculas y minúsculas
    }

        for (int i = 0; i < artist2.length(); ++i) {
        QChar c = artist2.at(i);
        if (!c.isLetter())
            artist2[i] = 'z' + 1;
        else
            artist2[i] = c.toLower();
        }

        return artist1 < artist2;
}

void setup() {
        Serial.begin(9600);

        // Inicializar la matriz de LEDs
        display.begin();
        display.setInvert(false);
        display.displayClear();

        // Inicializar el reproductor DFPlayer Mini
        mySoftwareSerial.begin(9600);
        if (!player.begin(mySoftwareSerial)) {
        Serial.println("No se pudo encontrar el reproductor DFPlayer Mini.");
        while (true);
        }

        player.volume(15); // Establecer el volumen del reproductor (0-30)

        // Inicializar la matriz de LEDs
        for (int i = 0; i < NUM_LEDS; i++) {
        matrix[i] = 0; // Apagar todos los LEDs
        }
}

void loop() {
        // Reproducir la canción
  player.play(1); // Reproducir la pista 1 desde la tarjeta microSD

        // Secuencia de encendido de LEDs
        for (int i = 0; i < NUM_LEDS; i++) {
        matrix[i] = 1; // Encender el LED actual
        display.displayAnimate();
        delay(100); // Ajusta el tiempo de acuerdo a la velocidad de la canción
    matrix[i] = 0; // Apagar el LED actual
        }

        // Detener la reproducción al finalizar la canción
  while (player.readState() != DFRobotDFPlayerMini::PLAY_END);
        player.stop();
}

void loop() {
        int reading = digitalRead(buttonPin);
        if (reading != lastButtonState) {
    lastDebounceTime = millis();
        }

        if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
            buttonState = reading;

            if (buttonState == LOW) {
                // El botón se presionó, cambiar entre pausa y reproducción
        if (player.readState() == DFRobotDFPlayerMini::PLAYING) {
                    player.pause();
                    Serial.println("Canción pausada.");
        } else {
                    player.start();
                    Serial.println("Canción en reproducción.");
        }
                delay(200); // Debounce para evitar múltiples pulsaciones rápidas
      }
    }
        }

        lastButtonState = reading;
}


