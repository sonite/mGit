#ifndef LANGUAGE_H
#define LANGUAGE_H

//** G L O B A L  D E F I N I T I O N S ***************************************/
//*****************************************************************************/

//** G L O B A L  V A R I A B L E S *******************************************/
static const char rom Button_Ok[] = "Ok";
#ifdef SWE
	static const char rom Button_Save[] = "Spara";
	static const char rom Button_Cancel[] = "Avbryt";
	static const char rom Button_Force[] = "Tvinga";
	static const char rom AnswerSave[] = "SPARAT";
	static const char rom AnswerCancel[] = "AVBRUTET";
	static const char rom AnswerTimeDate[] = " DATUM OCH TID:  ";
	static const char rom AnswerAlarm[] = "LARM!";
	static const char rom MenuTitle[] = "----- HUVUDMENY -----";
	static const char rom SubMenuTitle[] = "----- A/D  MENY -----";
#else
	static const char rom Button_Save[] = "Save";
	static const char rom Button_Cancel[] = "Cancel";
	static const char rom Button_Force[] = "Force";
	static const char rom AnswerSave[] = "SAVED";
	static const char rom AnswerCancel[] = "ABORTED";
	static const char rom AnswerTimeDate[] = " DATE AND TIME:  ";
	static const char rom AnswerAlarm[] = "       ALARM!        ";
	static const char rom MenuTitle[] = "----- MAIN MENU -----";
	static const char rom SubMenuTitle[] = "----- A/D  MENU -----";
#endif

//*****************************************************************************/

#endif
