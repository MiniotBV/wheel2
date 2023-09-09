enum errors
{
	E_GEEN = 0,

	E_NAALD_TERUG_GELOPEN   = 2,
	E_NAALD_NIET_BEWOGEN    = 3,
	E_PLAAT_NIET_OPGANG     = 4,
	E_KON_NIET_HOMEN        = 5,
	E_ARMHOEK_LIMIET_POS    = 6,
	E_ARMHOEK_LIMIET_NEG    = 7,
	E_TE_GROTE_UITSLAG      = 8,
};

enum errors error = E_GEEN;

Interval errorVeranderd(0, MILLIS);

String printError(enum errors s){
	if( s == E_GEEN                   ){ return "E_GEEN";}

	if( s == E_NAALD_TERUG_GELOPEN    ){ return "E_NAALD_TERUG_GELOPEN";}
	if( s == E_NAALD_NIET_BEWOGEN     ){ return "E_NAALD_NIET_BEWOGEN";}
	if( s == E_PLAAT_NIET_OPGANG      ){ return "E_PLAAT_NIET_OPGANG";}
	if( s == E_KON_NIET_HOMEN         ){ return "E_KON_NIET_HOMEN";}
	if( s == E_ARMHOEK_LIMIET_POS     ){ return "E_ARMHOEK_LIMIET_POS";}
	if( s == E_ARMHOEK_LIMIET_NEG     ){ return "E_ARMHOEK_LIMIET_NEG";}
	if( s == E_TE_GROTE_UITSLAG       ){ return "E_TE_GROTE_UITSLAG";}

	return "??????";
}

void setError(enum errors err)
{
	error = err;
	errorVeranderd.reset();
	Serial.println("V error: " + printError(err));
  Serial.println("  staat: " + printStaat(staat));
  Serial.println("  karPos: " + String(karPos));
  Serial.println("^");
}
