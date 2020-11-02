#include<curses.h>
#include <time.h>
#include<string.h>
#include<stdlib.h>
int r, c, nrows, ncols, nrowsgame, ncolsgame;
WINDOW *wndmenu, *gamewnd;
FILE * fisiertitlu;
char formatstring[30][30];
char varcorect;
int scor = 0;
int nrgresit = 0, nrcorect = 0;
int skip = 1, fifty = 1;



//functie de afisat animatia titlului
void titlu(int cadru)
{
	FILE * fisiertitlu;
	char buffer [200];
	fisiertitlu = fopen("titlu.txt", "r");
	while (fgets(buffer, 200, fisiertitlu) != NULL)
	{
		for (int i = 0; i < cadru; i++)
			printw(".");
		printw("%s", buffer);
	}
	move(0, 0);
}


//functie de delay, folosita la animatia titlului
void delay(int nr)
{
	clock_t start_time = clock();
	while (clock() < start_time + 1000 * nr);
}

//functie care afiseaza timpul curent pe ecran
void updateTimp()
{
	time_t mytime;
	mytime = time(NULL);
	mvprintw(nrows - 4, ncols / 10 - 2, ctime(&mytime));
}


//functie care afiseaza meniul, navigheaza prin el si returneaza selectia la ENTER
int menu(char **meniu, int row, int col, int arraylength, int selection)
{
	int i, ky = 0;
	char formatstring[15][15];

	//copiez cele 3 elemente din meniu( New Game, Resume Game, Quit)
	sprintf(formatstring[0], "%-s", meniu[0]);
	sprintf(formatstring[1], "%-s", meniu[1]);
	sprintf(formatstring[2], "%-s", meniu[2]);

	while (1)
	{
		//scriu pe ecran meniul,iar varianta selectata este evidentiata
		for (i = 0; i < 3; i++)
		{
			if (i == selection)
				attron(A_BOLD);
			mvprintw(row + i, col, formatstring[i], meniu[i]);
			attroff(A_BOLD);
		}

		//verific tasta apasata
		ky = getch();
		switch (ky)
		{
		case KEY_UP:
		{
			updateTimp();
			if (selection) //daca se poate muta selectia in sus
			{
				selection--;
			}
			break;
		}

		case KEY_DOWN:
		{
			updateTimp();
			if (selection < arraylength - 1) //daca se poate muta selectia in jos
			{
				selection++;
			}
			break;
		}

		case 10: //enter
		{
			updateTimp();
			return selection;

		}
		default:
		{
			updateTimp();
			break;
		}

		}
	}
}
//functia citeste din fisier fiecare intrebare cu raspunsurile ei,navigheaza
//printre variante, returneaza selectia si pune in aplicare variantele ajutatoare
int game(int row, int col, int arraylength, int selection, int *corect, int jocnou)
{
	int ultimaintrebare = 0, x, y;
	int counter, ky = 0;
	char buffer [200];
	//daca se incepe un joc nou
	if (jocnou == 0)
	{
		fseek(fisiertitlu, 0, 0); //prima linie din fisier
		fgets(buffer, 200, fisiertitlu); //citeste pe linii
		mvwprintw(gamewnd, row / 2, col / 2, buffer); //scrie pe ecran intrebarea
		fgets(buffer, 200, fisiertitlu);
		sprintf(formatstring[1], "%-s", buffer); //salveaza raspunsurile
		fgets(buffer, 200, fisiertitlu);
		sprintf(formatstring[2], "%-s", buffer);
		fgets(buffer, 200, fisiertitlu);
		sprintf(formatstring[3], "%-s", buffer);
		fgets(buffer, 200, fisiertitlu);
		sprintf(formatstring[4], "%-s", buffer);
		fgets(buffer, 200, fisiertitlu);
		varcorect = buffer[0]; //salveaza varianta corecta
	}
	//daca s-a mai raspuns deja la vreo intrebare
	else if (*corect == 1)
	{
		fgets(buffer, 200, fisiertitlu);
		mvwprintw(gamewnd, row / 2, col / 2, buffer);
		fgets(buffer, 200, fisiertitlu);
		sprintf(formatstring[1], "%-s", buffer);
		fgets(buffer, 200, fisiertitlu);
		sprintf(formatstring[2], "%-s", buffer);
		fgets(buffer, 200, fisiertitlu);
		sprintf(formatstring[3], "%-s", buffer);
		fgets(buffer, 200, fisiertitlu);
		sprintf(formatstring[4], "%-s", buffer);
		fgets(buffer, 200, fisiertitlu);
		varcorect = buffer[0];
	}
	//daca varianta corecta e 5, s-a terminat jocul
	if ((int)varcorect - '0' == 5)
	{
		selection = 0;
		ultimaintrebare = 1;
		mvwprintw(gamewnd, row, col - 15, "Ai terminat! Apasa ENTER.");
	}

	//la fel ca la functia menu
	while (ky != 27)
	{

		for (counter = 0; counter <= 3; counter++)
		{
			if (counter == selection)
				wattron(gamewnd, A_BOLD);
			mvwprintw(gamewnd, row + counter + ultimaintrebare, col, formatstring[counter + 1]);

			wattroff(gamewnd, A_BOLD);
		}
		wrefresh(gamewnd);
		ky = getch();
		switch (ky)
		{
		case KEY_UP:
		{
			if (selection)
			{
				updateTimp();
				selection--;
			}
			break;
		}
		case KEY_DOWN:
		{
			updateTimp();
			if (selection < arraylength - 1)
			{
				selection++;
			}
			break;
		}
		case 'q': //daca se apasa q se iese
		{
			updateTimp();
			*corect = -1;
			return selection;
		}

		case 10: //enter
		{
			updateTimp();
			if (ultimaintrebare == 1) //daca s-a terminat fisierul de intrebari
			{
				return -2;
			}
			if (selection == (int)varcorect - '0') //daca raspunsul e corect
				*corect = 1;
			else *corect = 0;
			return selection;
		}
		case 's': //variantta ajutatoare skip
		{
			updateTimp();
			if (skip == 1 && ultimaintrebare == 0) //daca nu s-a mai apasat "s"
			{	//si nu s-a terminat fisierul
				mvprintw(nrows - 3, ncols / 2 - 2, "Skip folosit!    ");
				*corect = 1;
				skip = 0;
				game(nrowsgame / 2 - 1, ncolsgame / 2 - 1, arraylength, 0, corect, 1); //skip
				if (*corect == 0) //daca s-a raspuns gresit
				{
					nrgresit++;
					scor = scor - 5;
				}
				else if (*corect == 1) //daca s-a raspuns gresit
				{
					nrcorect++;
					scor = scor + 10;
				}
				else if (*corect == -1) //daca s-a apasat 'q'
				{
					*corect = -3;
					return 0;
				}
				*corect = -2;
				return 0;


			}
			break;
		}

		case 'f': //variantta ajutatoare 50-50
		{
			updateTimp();
			if (fifty == 1) //daca nu s-a mai apasat "f"
			{
				srand(time(NULL)); //seed random
				fifty = 0;
				x = rand() % 4;	// random de la 0-3
				y = rand() % 4;

				//cat timp x si y sunt acelasi random
				//sau sunt varianta corecta a intrebarii
				while (x == y || (x == (int)varcorect - '0' || y == (int)varcorect - '0') )
				{
					x = rand() % 4;
					y = rand() % 4;
				}
				//se suprascriu variantele gresite
				strcpy(formatstring[x + 1], "-----------");
				strcpy(formatstring[y + 1], "-----------");
				mvprintw(nrows - 2, ncols / 2 - 2, "50/50 folosit!    ");

			}
			break;
		}

		default:
		{
			updateTimp();
			break;
		}

		}
	}
}

//functia verifica selectia meniului si jocului,calculeaza scorul si nr de raspunsuri corecte si gresite
int selectionwindow( char **array,  int row,  int col,  int arraylength, int menulength, int *ok, int *x)
{

	updateTimp();
	int y, corect, gresit, i;
	int ky;
	char stringscor[40];

	wndmenu = initscr();
	keypad(stdscr, TRUE);
	int selection = menu(array, row, col, arraylength - 1, 0); //selectia meniului
	refresh();

	if (selection == 2) //daca s-a apasat Quit
		endwin();
	else if (selection == 0) //daca s-a apasat New Game
	{
		mvprintw(nrows - 3, ncols / 2 - 2, "Skip Valabil!(s)");
		mvprintw(nrows - 2, ncols / 2 - 2, "50/50 Valabil!(f)");
		skip = 1;
		fifty = 1;
		nrgresit = 0;
		nrcorect = 0;
		mvprintw(3, ncols - 25, "                     ");
		mvprintw(4, ncols - 25, "                     ");
		mvprintw(5, ncols - 25, "                     ");
		*ok = 1;
		scor = 0;
		gamewnd = newwin(nrows - 20, ncols - 20, 10, 15); //se creeaza fereastra de game
		wrefresh(gamewnd);
		getmaxyx(gamewnd, nrowsgame, ncolsgame);
		y = game(nrowsgame / 2 - 1, ncolsgame / 2 - 1, arraylength, 0, &corect, 0); //selectia jocului
		*x = y;

		if (corect == 0) //raspuns gresit din prima(nu ar intra in while-ul de mai jos)
		{
			corect = 1;
			gresit = 1;
			nrgresit++;
		}
		else if (corect == 1) nrcorect++; //corect din prima
		while (corect == 1 || corect == -2 || corect == -3)
		{
			if (gresit == 1) //daca raspunsul a fost gresit din prima
			{
				scor = -10;
				gresit = 0;
			}
			if (corect == -3)
			{
				break;
			}
			if (corect != -2)
				scor = scor + 10;

			corect = 1;
			y = game(nrowsgame / 2 - 1, ncolsgame / 2 - 1, arraylength, y, &corect, 1);//selectia jocului
			*x = y;
			if (y == -2) //daca s-a terminat jocul
			{
				wclear(gamewnd);
				wrefresh(gamewnd);
				sprintf(stringscor, "Scor=%-d", scor); //Afisarea scorului si a nr de var corecte/gresite
				mvprintw(3, ncols - 25, stringscor);
				sprintf(stringscor, "Raspunsuri corecte=%-d", nrcorect);
				mvprintw(4, ncols - 25, stringscor);
				//if(skip==0)
				//nr--;
				sprintf(stringscor, "Raspunsuri gresite=%-d", nrgresit);
				mvprintw(5, ncols - 25, stringscor);
				break;
			}

			if (corect == -1)
			{
				break;
			}
			if (corect == 0)
			{
				nrgresit++;
				if (scor >= 10)
					scor = scor - 15;
				else scor = -10;
				corect = 1;
			}
			else if (corect == 1)
				nrcorect++;


		}

		wrefresh(wndmenu);
		selectionwindow(array, row, col, arraylength, 4, ok, x); //se ia de la inceput functia


	}
	else if (selection == 1 && *ok == 1) //daca s-a apasat pe Resume dupa apasarea New Game
	{
		y = *x;
		wrefresh(gamewnd);
		y = game(nrowsgame / 2 - 1, ncolsgame / 2 - 1, arraylength, y, &corect, 1);

		*x = y;
		if (y != -2) //daca nu s-a terminat jocul
		{
			if (corect != -1)
			{
				//in continuare, la fel ca la New Game
				if (corect == 0)
				{
					corect = 1;
					gresit = 1;
					nrgresit++;
				}
				else if (corect == 1)
					nrcorect++;;
				while (corect == 1 || corect == -2 || corect == -3)
				{
					if (gresit == 1)
					{
						scor = scor - 15;
						gresit = 0;
					}
					if (corect == -3)
					{
						break;
					}
					if (corect != -2)
						scor = scor + 10;

					y = game(nrowsgame / 2 - 1, ncolsgame / 2 - 1, arraylength, y, &corect, 1);
					*x = y;
					if (y == -2)
					{
						sprintf(stringscor, "Scor=%-d", scor);
						mvprintw(3, ncols - 25, stringscor);
						if (nrcorect == 0)
						{
							nrcorect = 1;
							nrgresit--;
						}
						sprintf(stringscor, "Raspunsuri corecte=%-d", nrcorect);
						mvprintw(4, ncols - 25, stringscor);
						sprintf(stringscor, "Raspunsuri gresite=%-d", nrgresit);
						mvprintw(5, ncols - 25, stringscor);
						break;
					}
					if (corect == -1)
					{
						break;
					}
					if (corect == 0)
					{
						nrgresit++;
						if (scor >= 10)
							scor = scor - 15;
						else scor = -10;
						corect = 1;
					}
					else  if (corect == 1)nrcorect++;

				}
			}
		}
		wrefresh(wndmenu);
		selectionwindow(array, row, col, arraylength, 4, ok, x);

	}
	else //daca s-a apasat Resume fara New Game inainte
	{
		selectionwindow(array, row, col, arraylength, 4, ok, x);
	}

}
int main(int argc, char const *argv[])

{
	if (argv[1] == NULL)
	{
		fprintf(stderr, "Nu s-au dat argumente in linia de comanda.\n");
		return 1;
	}
	fisiertitlu = fopen(argv[1], "r");
	int x = 0, ky, y, corect = 0;
	char *testarray[] = {"New Game", "Resume Game", "Quit"};
	int ok = 0;
	wndmenu = initscr();

	//Colorarea fundalului
	start_color();
	bkgdset(COLOR_BLACK);

	cbreak();
	noecho();
	getmaxyx(wndmenu, nrows, ncols);
	clear();
	refresh();
	curs_set(0);

	for (int j = 1; j <= 3; j++) //animatia titlului
		for (int i = 0; i <= 3; i++)
		{
			titlu(i);
			refresh();
			delay(400);

		}

	refresh();
	selectionwindow(testarray, 10, 1, 44, 5, &ok, &y);


	return 1;

}