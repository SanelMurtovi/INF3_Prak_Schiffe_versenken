void randomStrategy(TASK3::World& world) {
    int x, y; // Variablen für die Schusskoordinaten
    TASK3::ShootResult result; // Variable für das Ergebnis eines Schusses

    do {
        x = rand() % world.getMaxX() + 1; // Zufällige x-Koordinate im Bereich 1 bis maxX
        y = rand() % world.getMaxY() + 1; // Zufällige y-Koordinate im Bereich 1 bis maxY

        // Prüfe, ob die Koordinate noch nicht verwendet wurde
        if (world.coordAlreadyUsed(x, y) == TASK3::BLOCK_CLEAR) {
            result = world.shoot(x, y); // Führe einen Schuss aus
            cout << "Random Shot: (" << x << ", " << y << ") --> " << result << endl; // Ausgabe des Ergebnisses
            world.printBoard(); // Zeige das Spielfeld nach jedem Schuss an
        }
    } while (result != TASK3::GAME_OVER); // Wiederhole, bis das Spiel vorbei ist
}