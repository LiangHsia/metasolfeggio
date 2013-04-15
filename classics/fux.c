/* fux.c */

/* ftp://ccrma-ftp.stanford.edu/pub/Lisp/fux.txt */

/* AUTOMATIC COUNTERPOINT
 *
 * from "Automatic Species Counterpoint", Bill Schottstaedt, CCRMA STAN-M-19, May 1984
 * translated from SAIL to C in March 1995
 *
 * This file can be treated as a stand-alone C program or as a foreign function module for Lisp.
 *
 * make fux creates the C program
 * cc fux.c -c -O -DCM creates the module
 *
 * See the "main" function for examples, or fux.lisp (which ties fux.c into CMN/CM).
 */

#include <stdio.h>

#ifndef inline
#define inline
#endif

inline int ABS(int i) {if (i < 0) return(-i); else return(i);}
inline int MIN(int a, int b) {if (a < b) return(a); else return(b);}
inline int MAX(int a, int b) {if (a > b) return(a); else return(b);}
inline void ARRBLT(int *dest, int *source, int num) {int i; for (i=0;i<num;i++) dest[i]=source[i];} /* SAIL p51 */

#define Unison 0
#define MinorSecond 1
#define MajorSecond 2
#define MinorThird 3
#define MajorThird 4
#define Fourth 5
#define Tritone 6
#define Fifth 7
#define MinorSixth 8
#define MajorSixth 9
#define MinorSeventh 10
#define MajorSeventh 11
#define Octave 12

int PerfectConsonance[13] =   {1,0,0,0,0,0,0,1,0,0,0,0,1};
int ImperfectConsonance[13] = {0,0,0,1,1,0,0,0,1,1,0,0,0};
int Dissonance[13] =          {0,1,1,0,0,1,1,0,0,0,1,1,0};

#define Aeolian 1
#define Dorian 2
#define Phrygian 3
#define Lydian 4
#define Mixolydian 5
#define Ionian 6
#define Locrian 7

int _Ionian[12] =     {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
int _Dorian[12] =     {1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0};
int _Phrygian[12] =   {1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0};
int _Lydian[12] =     {1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1};
int _Mixolydian[12] = {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0};
int _Aeolian[12] =    {1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0};
int _Locrian[12] =    {1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0};

inline int InMode(int Pitch, int Mode)
{
  int pit;
  if (Pitch>11)
    pit = Pitch % 12;
  else pit=Pitch;
  switch (Mode)
    {
    case Ionian:     return(_Ionian[pit]); break;
    case Aeolian:    return(_Aeolian[pit]); break;
    case Dorian:     return(_Dorian[pit]); break;
    case Phrygian:   return(_Phrygian[pit]); break;
    case Lydian:     return(_Lydian[pit]); break;
    case Mixolydian: return(_Mixolydian[pit]); break;
    case Locrian:    return(_Locrian[pit]); break;
    }
  return(0);
}

int BadMelodyInterval[13] = {0,0,0,0,0,0,1,0,0,1,1,1,0};
inline int BadMelody(int Intv) {return( ((ABS(Intv)>Octave) || BadMelodyInterval[ABS(Intv)]) || (Intv == -MinorSixth));}

inline int ASkip(int Interval) {return(ABS(Interval)>MajorSecond);}
inline int AStep(int Interval) {return((ABS(Interval) == MinorSecond) || (ABS(Interval) == MajorSecond));}

inline int AThird(int Interval) {return((Interval == MinorThird) || (Interval == MajorThird));}
inline int ASeventh(int Interval) {return((Interval == MinorSeventh) || (Interval == MajorSeventh));}
inline int AnOctave(int Interval) {return((Interval != Unison) && ((ABS(Interval) % 12) == 0));}
inline int ATenth(int Interval) {return((ABS(Interval)>14) && (AThird((ABS(Interval) % 12))));}

#define DirectMotion 1
#define ContraryMotion 2
#define ObliqueMotion 3
#define NoMotion 4

inline int MotionType(int Pitch1, int Pitch2, int Pitch3, int Pitch4)
{
  if ((Pitch1 == Pitch2) || (Pitch3 == Pitch4))
    {
      if ((Pitch1 == Pitch2) && (Pitch3 == Pitch4))
	return(NoMotion);
      else return(ObliqueMotion);
    }
  else
    {
      if ((Pitch2-Pitch1)*(Pitch4-Pitch3)>0)
	return(DirectMotion);
      else return(ContraryMotion);
    }
}

inline int DirectMotionToPerfectConsonance(int Pitch1, int Pitch2, int Pitch3, int Pitch4)
{
  return(PerfectConsonance[ABS(Pitch4-Pitch2) % 12] && (MotionType(Pitch1,Pitch2,Pitch3,Pitch4) == DirectMotion));
}
	
inline int ConsecutiveSkipsInSameDirection(int Pitch1, int Pitch2, int Pitch3)
{
  return( (((Pitch1>Pitch2) && (Pitch2>Pitch3)) || ((Pitch1<Pitch2) && (Pitch2<Pitch3))) &&
	  (ASkip(Pitch2-Pitch1) && ASkip(Pitch3-Pitch2)));
}

#define HighestSemitone 72
#define LowestSemitone 24

inline int OutOfRange(int Pitch) {return((Pitch>HighestSemitone) || (Pitch<LowestSemitone));}
inline int ExtremeRange(int Pitch) {return(Pitch>(HighestSemitone-3) || Pitch<(LowestSemitone+3));}

#define MostNotes 128
#define MostVoices 6

int BasePitch,Mode,TotalTime;

int Ctrpt[MostNotes][MostVoices];
int Onset[MostNotes][MostVoices];
int Dur[MostNotes][MostVoices];
int TotalNotes[MostVoices];

inline int Us(int n, int v) {return(Ctrpt[n][v]);}
inline int LastNote(int n, int v) {return(n == TotalNotes[v]);}
inline int FirstNote(int n, int v) {return(n == 1);}
inline int NextToLastNote(int n, int v) {return(n == (TotalNotes[v]-1));}
inline void SetUs(int n, int p, int v) {Ctrpt[n][v]=p;}

inline int TotalRange(int Cn, int Cp, int v)
{
  int Minp,Maxp,i,pit;
  Minp=Cp;
  Maxp=Cp;
  for (i=1;i<Cn;i++)
    {
      pit=Us(i,v);
      Minp=MIN(Minp,pit);
      Maxp=MAX(Maxp,pit);
    }
  return(Maxp-Minp);
}

inline int Cantus(int n, int v) {return(Ctrpt[((Onset[n][v]) >> 3) + 1][0]);}

inline int VIndex(int Time, int VNum)
{
  int i;
  for (i=1;i<TotalNotes[VNum];i++)
    if ((Onset[i][VNum] <= Time) && ((Onset[i][VNum]+Dur[i][VNum])>Time)) return(i);
  return(i);
}
        
inline int Other(int Cn, int v, int v1) {return(Ctrpt[VIndex(Onset[Cn][v],v1)][v1]);}

inline int Bass(int Cn, int v)
{
  int j,LowestPitch;
  LowestPitch=Cantus(Cn,v);
  for (j=1;j<v;j++) LowestPitch=MIN(LowestPitch,Other(Cn,v,j));
  return(LowestPitch);
}

#define WholeNote 8
#define HalfNote 4
#define DottedHalfNote 6
#define QuarterNote 2
#define DottedQuarterNote 3
#define EighthNote 1       

inline int Beat8(int n) {return(n % 8);}
inline int DownBeat(int n, int v) {return(Beat8(Onset[n][v]) == 0);}
inline int UpBeat(int n, int v) {return(!(DownBeat(n,v)));}

inline int PitchRepeats(int Cn, int Cp, int v)
{
  int i,k;
  i=0;
  for (k=1;k<Cn;k++) {if (Us(k,v) == Cp) i++;}
  return(i);
}

#define One 0
#define Two 2
#define Three 3
#define Four 4
#define Five 5
#define Six 6
#define Eight 8

inline int Size(int MelInt)
{
  int ActInt,IntTyp;
  ActInt=ABS(MelInt);
  switch (ActInt)
    {
    case Unison: IntTyp=One; break;
    case MinorSecond: case MajorSecond: IntTyp=Two; break;
    case MinorThird: case MajorThird: IntTyp=Three; break;
    case Fourth: IntTyp=Four; break;
    case Fifth: IntTyp=Five; break;
    case MinorSixth: IntTyp=Six; break;
    case Octave: IntTyp=Eight; break;
    }
  if (MelInt>0) return(IntTyp);
  else return(-IntTyp);
}

int TooMuchOfInterval(int Cn, int Cp, int v)
{
  int Ints[17];
  int i,k,MinL;
  for (i=0;i<17;i++) Ints[i]=0;
  for (i=2;i<Cn;i++)
    {
      k=(Size(Ctrpt[i][v]-Ctrpt[i-1][v])+8);
      Ints[k]++;
    }
  k=(Size(Cp-Ctrpt[Cn-1][v])+8);
  MinL=0;
  for (i=1;i<17;i++) {if ((i != k) && (Ints[i]>Ints[MinL])) MinL=i;}
  return(Ints[k]>(Ints[MinL]+6));
}

int ADissonance(int Interval, int Cn, int Cp, int v, int Species)
{
  int MelInt;
  if ((Species == 1) || (Dur[Cn][v] == WholeNote))
    return(Dissonance[Interval]);
  else
    {
      if (Species == 2)
	{
	  if (DownBeat(Cn,v) || (!(AStep(Cp-Us(Cn-1,v)))))
	    return(Dissonance[Interval]);
	  else return(0);
	}
      else
	{
	  if (Species == 3)
	    {
	      if ((Beat8(Onset[Cn][v]) == 0) || (FirstNote(Cn,v) || LastNote(Cn,v)))
		return(Dissonance[Interval]);
	      MelInt=(Cp-Us(Cn-1,v));
	      if (!(AStep(MelInt))) return(Dissonance[Interval]);
	      /* 0 cannot be dissonant (downbeat)
	       * 1 can be if passing either way, but must be approached by step.
	       * 2 can be if passing 2 to 4 (both latter cons)
	       * 3 can be if passing but must be approached and left by step
	       */
	      return(0);
	    }
	  else
	    {
	      if (Species == 4)
		{
		  if (UpBeat(Cn,v) || (FirstNote(Cn,v) || LastNote(Cn,v)))
		    return(Dissonance[Interval]);
		  MelInt=(Cp-Us(Cn-1,v));
		  if (MelInt != 0) return(Dissonance[Interval]);
		  return(0);	/* i.e. unison to downbeat is ok, but needs check later */
		}
	      else
		{
		  if (Species == 5)
		    {
		      if (Beat8(Onset[Cn][v]) == 0)
			{
			  if (Cp == Us(Cn-1,v)) return(0);
			  else return(Dissonance[Interval]);
			}
		      else
			{
			  if (!(AStep(Cp-Us(Cn-1,v)))) return(Dissonance[Interval]);
			  return(0);
			}
		    }
		}
	    }
	}
    }
  return(0);
}

int Doubled(int Pitch, int Cn, int v)
{
  int VNum;
  for (VNum=0;VNum<v;VNum++)
    {
      if ((Other(Cn,v,VNum) % 12) == Pitch) return(1);
    }
  return(0);
}

#define infinity 1000000
#define Bad 100
#define RealBad 200

#define UnisonPenalty		        Bad
#define DirectToFifthPenalty		RealBad
#define DirectToOctavePenalty		RealBad
#define ParallelFifthPenalty		infinity
#define ParallelUnisonPenalty		infinity
#define EndOnPerfectPenalty		infinity
#define NoLeadingTonePenalty		infinity
#define DissonancePenalty		infinity
#define OutOfRangePenalty		RealBad
#define OutOfModePenalty		infinity
#define TwoSkipsPenalty			1
#define DirectMotionPenalty		1
#define PerfectConsonancePenalty	2
#define CompoundPenalty			1
#define TenthToOctavePenalty		8
#define SkipTo8vePenalty		8
#define SkipFromUnisonPenalty		4
#define SkipPrecededBySameDirectionPenalty	1
#define FifthPrecededBySameDirectionPenalty	3
#define SixthPrecededBySameDirectionPenalty	8
#define SkipFollowedBySameDirectionPenalty	3
#define FifthFollowedBySameDirectionPenalty	8
#define SixthFollowedBySameDirectionPenalty	34
#define TwoSkipsNotInTriadPenalty	3
#define BadMelodyPenalty		infinity
#define ExtremeRangePenalty		5
#define LydianCadentialTritonePenalty	13
#define UpperNeighborPenalty		1
#define LowerNeighborPenalty		1
#define OverTwelfthPenalty		infinity
#define OverOctavePenalty		Bad
#define SixthLeapPenalty		2
#define OctaveLeapPenalty		5
#define BadCadencePenalty		infinity
#define DirectPerfectOnDownbeatPenalty	infinity
#define RepetitionOnUpbeatPenalty	Bad
#define DissonanceNotFillingThirdPenalty	infinity
#define UnisonDownbeatPenalty		3
#define TwoRepeatedNotesPenalty		2
#define ThreeRepeatedNotesPenalty	4
#define FourRepeatedNotesPenalty	7
#define LeapAtCadencePenalty		13
#define NotaCambiataPenalty		infinity
#define NotBestCadencePenalty		8
#define UnisonOnBeat4Penalty		3
#define NotaLigaturePenalty		21
#define LesserLigaturePenalty           8
#define UnresolvedLigaturePenalty	infinity
#define NoTimeForaLigaturePenalty	infinity
#define EighthJumpPenalty		Bad
#define HalfUntiedPenalty		13
#define UnisonUpbeatPenalty		21
#define MelodicBoredomPenalty		1
#define SkipToDownBeatPenalty		1
#define ThreeSkipsPenalty		3
#define DownBeatUnisonPenalty		Bad
#define VerticalTritonePenalty		2
#define MelodicTritonePenalty		8
#define AscendingSixthPenalty		1
#define RepeatedPitchPenalty		1
#define NotContraryToOthersPenalty	1
#define NotTriadPenalty			34
#define InnerVoicesInDirectToPerfectPenalty	21
#define InnerVoicesInDirectToTritonePenalty	13
#define SixFiveChordPenalty		infinity
#define UnpreparedSixFivePenalty	Bad
#define UnresolvedSixFivePenalty	Bad
#define AugmentedIntervalPenalty	infinity
#define ThirdDoubledPenalty		5
#define DoubledLeadingTonePenalty	infinity
#define DoubledSixthPenalty		5
#define DoubledFifthPenalty		3
#define TripledBassPenalty		3
#define UpperVoicesTooFarApartPenalty	1
#define UnresolvedLeadingTonePenalty	infinity
#define AllVoicesSkipPenalty		8
#define DirectToTritonePenalty		Bad
#define CrossBelowBassPenalty		infinity

/* I added the following during the translation to C */
#define CrossAboveCantusPenalty		infinity
#define NoMotionAgainstOctavePenalty    34

int SpecialSpeciesCheck(int Cn, int Cp, int v, int Other0, int Other1, int Other2, int NumParts,
			int Species, int MelInt, int Interval, int ActInt, int LastIntClass, int Pitch, int LastMelInt, int CurLim)
{
  int Val,Above,i,LastDisInt;
  if (Species == 1) return(0);	/* no special rules for 1st species */
  Val=0;
  if (Species == 2)
    {
      if ((NextToLastNote(Cn,v)) && ((Pitch == 11) || (Pitch == 10)))
	{
	  if ((Mode != Phrygian) || (Interval >= 0))
	    {
	      if (LastIntClass !=  Fifth) Val += BadCadencePenalty;
	    }
	  else
	    {
	      if (LastIntClass != MinorSixth) Val += BadCadencePenalty;
	    }
	}
    }
  else
    {
      if (Species == 4)
	{
	  if ((DownBeat(Cn,v)) && (MelInt != Unison)) Val += NotaLigaturePenalty;
	  if ((UpBeat(Cn,v)) && (Dissonance[LastIntClass]))
	    {
	      if ((MelInt != (-MinorSecond)) && (MelInt != (-MajorSecond))) Val += UnresolvedLigaturePenalty;
	      if ((ActInt == Unison) && ((Interval<0) || (((ABS(Us(Cn-2,v)-Other2)) % 12) == Unison))) Val += NoTimeForaLigaturePenalty;
	      if ((ActInt == Fifth) || (ActInt == Tritone)) Val += NoTimeForaLigaturePenalty;
	    }
	}
      else
	{
	  Above=(Interval >= 0);
	  
	  /* added check to stop optimizer from changing 4th beat passing tones into repeated notes+skip */
	  if (((Beat8(Onset[Cn][v]) == 6) || (Beat8(Onset[Cn][v]) == 7)) && (Cp == Us(Cn-1,v))) Val += UnisonOnBeat4Penalty;
	  
	  /* skip to down beat seems not so great */
	  if (Beat8(Onset[Cn][v]) == 0)
	    {
	      if (ASkip(MelInt)) Val += SkipToDownBeatPenalty;
	      if ((Cn>2) && ((ActInt == Unison) || (ActInt == Fifth)))
		{
		  if (Species == 5)
		    {
		      i=(Cn-1);
		      while ((i>0) && ((Beat8(Onset[i][v])) != 0)) i--;
		    }
		  else i=(Cn-4);
		  if (((ABS(Us(i,v)-Bass(i,v))) % 12) == ActInt) Val += DownBeatUnisonPenalty;
		}
	    }
	  
	  /* check for cambiata not resolved correctly (on 4th beat) */
	  if ((Beat8(Onset[Cn][v]) == 6) && 
	      ((AThird(ABS(LastMelInt))) &&
	       ((Dissonance[(ABS(Us(Cn-2,v)-Other2)) % 12]) &&
		((MelInt<0) || ((ABS(MelInt) != MajorSecond) && (ABS(MelInt) != MinorSecond))))))
	    Val += NotaCambiataPenalty;
	  if (Val >= CurLim) return(Val);
	  
	  if ((Species == 3) && ((Cn>1) && (Dissonance[LastIntClass])))
	    {
	      switch (Beat8(Onset[Cn][v]))
		{
		case 0: case 6:
		  if ((!(AStep(MelInt))) || ((!(AStep(LastMelInt))) || ((MelInt*LastMelInt)<0))) Val += DissonancePenalty;
		  break;
		case 2:
		  Val += DissonancePenalty;
		  break;
		case 4:
		  if ((!(AStep(LastMelInt))) || ((ABS(MelInt)>MajorThird) || ((MelInt == 0) || ((LastMelInt*MelInt)<0))))
		    Val += DissonancePenalty;
		  else
		    {
		      if (!(AStep(MelInt)))
			{
			  if (Above)
			    {
			      if (!(ASeventh(LastIntClass))) Val += DissonancePenalty;
			    }
			  else
			    {
			      if (LastIntClass != Fourth) Val += DissonancePenalty;
			    }
			}
		    }
		  break;
		}
	    }
	  
	  if (Species == 5)
	    {
	      if ((Cn>1) && ((Beat8(Onset[Cn][v]) == 0) && ((Cp != Us(Cn-1,v)) && (Dur[Cn][v] <= Dur[Cn-1][v]))))
		Val += LesserLigaturePenalty;
	      if ((Cn>3) && ((Dur[Cn][v] == HalfNote) && ((Beat8(Onset[Cn][v]) == 4) &&
		  ((Dur[Cn-1][v] == QuarterNote) && (Dur[Cn-2][v] == QuarterNote)))))
		Val += HalfUntiedPenalty;
	      if ((Dur[Cn][v] == EighthNote) && ((DownBeat(Cn,v)) && (Dissonance[ActInt])))
		Val += DissonancePenalty;
	      if (Val >= CurLim) return(Val);
	      if (Cn>1) {LastDisInt = ((ABS(Us(Cn-1,v)-Other1)) % 12);}
	      if ((Cn>1) && (Dissonance[LastDisInt]))
		{
		  switch (Beat8(Onset[Cn-1][v]))
		    {
		    case 6: case 4:
		      if (!((LastDisInt == Fourth) && ((MelInt == Unison) &&
			    (((Other0-Other1) == Unison) && (Beat8(Onset[Cn][v]) == 0)))))
			{
			  if ((!(AStep(MelInt))) || ((!(AStep(LastMelInt))) || 
			      (((MelInt*LastMelInt)<0) || ((Dur[Cn-1][v] == EighthNote) ||
			       ((Dur[Cn-1][v] == QuarterNote) && (Dur[Cn-2][v] == HalfNote))))))
			    Val += DissonancePenalty;
			}
		      break;
		    case 1: case 3: case 5: case 7:
		      if ((!(AStep(MelInt))) || ((!(AStep(LastMelInt))) || ((MelInt*LastMelInt)<0)))
			Val += DissonancePenalty;
		      break;
		    case 0:
		      if ((Dur[Cn-2][v] == EighthNote) || (Dur[Cn-2][v]<Dur[Cn-1][v])) Val += NoTimeForaLigaturePenalty;
		      if ((MelInt != (-MinorSecond)) && (MelInt != (-MajorSecond))) Val += UnresolvedLigaturePenalty;
		      if ((ActInt == Fourth) || (ActInt == Tritone)) Val += NoTimeForaLigaturePenalty;
		      if ((ActInt == Fifth) && (Interval<0)) Val += NoTimeForaLigaturePenalty;
		      if ((ActInt == 0) && (((ABS(Us(Cn-2,v)-Other2)) % 12) == 0)) Val += NoTimeForaLigaturePenalty;
		      if (LastMelInt != Unison) Val += DissonancePenalty;
		      break;
		    case 2:
		      if ((!(AStep(LastMelInt))) || ((ABS(MelInt)>MajorThird) ||
			  ((MelInt == 0) || ((Dur[Cn-1][v] == EighthNote) || ((LastMelInt*MelInt)<0)))))
			Val += DissonancePenalty;
		      else
			{
			  if (!(AStep(MelInt)))
			    {
			      if (Above)
				{   
				  if (!(ASeventh(LastIntClass))) Val += DissonancePenalty;
				}
			      else
				{
				  if (LastIntClass != Fourth) Val += DissonancePenalty;
				}
			    }
			}
		      break;
		    }
		}
	      if ((Cn>1) && ((Dur[Cn-1][v] == EighthNote) && (!(AStep(MelInt))))) Val += EighthJumpPenalty;
	      if ((Cn>1) && ((Dur[Cn-1][v] == HalfNote) && ((Beat8(Onset[Cn][v]) == 4) && (MelInt == Unison))))
		Val += UnisonUpbeatPenalty;
	    }
	}
    }
  return(Val);
}

#define INTERVALS_WITH_BASS_SIZE 8
int IntervalsWithBass[INTERVALS_WITH_BASS_SIZE];
    /* 0 = octave, 2 = step, 3 = third, 4 = fourth, 5 = fifth, 6 = sixth, 7 = seventh */

void AddInterval(int n)
{
  int ActInt;
  switch (n % 12)
    {
    case 0: ActInt = 0; break;
    case 1: case 2: ActInt = 2; break;
    case 3: case 4: ActInt = 3; break;
    case 5: case 6: ActInt = 4; break;
    case 7: ActInt = 5; break;
    case 8: case 9: ActInt = 6; break;
    case 10: case 11: ActInt = 7; break;
    }
  IntervalsWithBass[ActInt]++;
}

int OtherVoiceCheck(int Cn, int Cp, int v, int NumParts, int Species, int CurLim)
{
  int Val,k,CurBass,Other0,Other1,Int0,Int1,ActPitch,IntBass,LastCp,AllSkip,i,ourLastInt;
  if (v == 1) return(0);	/* two part or bass voice, so nothing to check */
  for (i=0;i<INTERVALS_WITH_BASS_SIZE;i++) IntervalsWithBass[i]=0;
  Val=0;
  CurBass=Bass(Cn,v);
  if (Cp <= CurBass) Val += CrossBelowBassPenalty;
  IntBass=((Cp-CurBass) % 12);
  if ((IntBass == MajorThird) && (!(InMode(CurBass,Mode)))) Val += AugmentedIntervalPenalty;
  ActPitch=(Cp % 12);
  
  if ((Val >= CurLim) || ((v == NumParts) && (Dissonance[IntBass]))) return(Val);
  /* logic here is that only the last part can be non-1st species
     and may therefore have various dissonances that don't want to be
     calculated as chord tones
     */
  LastCp=Us(Cn-1,v);
  AllSkip=ASkip(Cp-LastCp);
  AddInterval(IntBass);
  for (k=0;k<v;k++)
    {
      Other0=Other(Cn,v,k);
      Other1=Other(Cn-1,v,k);
      if (!(ASkip(Other0-Other1))) AllSkip=0;
      AddInterval(Other0-CurBass);	/* add up tones in chord */
      /* avoid unison with other voice */
      if ((!(LastNote(Cn,v))) && (Other0 == Cp)) Val += UnisonPenalty;

      /* keep upper voices closer together than lower */
      if ((Other0 != CurBass) && ((ABS(Cp-Other0)) >= (Octave+Fifth))) Val += UpperVoicesTooFarApartPenalty;

      /* check for direct motion to perfect consonance between these two voices */
      Int0=((ABS(Other0-Cp)) % 12);
      Int1=((ABS(Other1-LastCp)) % 12);
      if (Int1 == Int0)
	{
          if (Int0 == Unison) Val += ParallelUnisonPenalty;
	  else if (Int0 == Fifth) Val += ParallelFifthPenalty;
	}
      if ((Cn>2) && ((Int0 == Unison) && (((ABS(Us(Cn-2,v)-Other(Cn-2,v,k))) % 12) == Unison)))
        Val += ParallelUnisonPenalty;

      if (Val >= CurLim) return(Val);

      /* penalize tritones between voices */
      if (Int0 == Tritone) Val += VerticalTritonePenalty;

      if (Species == 5)
	{
          if ((Dissonance[Int1]) && (Int1 != Fourth))
	    {
              ourLastInt=((LastCp-Bass(Cn-1,v)) % 12);
              if (ourLastInt != Unison)	/* if unison, 6-6 somewhere else? */
		{
                  if (ourLastInt == Fifth)
		    {
                      if ((ASkip(Cp-LastCp)) || (Cp >= LastCp)) Val += UnresolvedSixFivePenalty;
		    }
		  else
		    {
		      if ((ASkip(Other0-Other1)) || (Other0 >= Other1)) Val += UnresolvedSixFivePenalty;
		    }
		}
	    }
          if ((Dissonance[Int0]) && ((Int0 != Fourth) && (IntBass != Unison)))
	    {
              if ((IntBass == Fifth && ((Cp-LastCp) != Unison)) ||
		  ((IntBass != Fifth) && ((Other0-Other1) != Unison)))
		Val += UnpreparedSixFivePenalty;
	    }
	}

      /* penalize direct motion to perfect consonance except at the cadence */
      if ((!(LastNote(Cn,v))) && (DirectMotionToPerfectConsonance(LastCp,Cp,Other1,Other0)))
	Val += InnerVoicesInDirectToPerfectPenalty;

      /* if we have an unraised leading tone it is possible that some other
       * voice has the raised form thereof (since the voices can move at very
       * different paces, one voice's next to last note may be long before
       * another's) 
       */
      if ((ActPitch == 10) &&	 	        /* if 11 we've aready checked */
	  ((Other0 % 12) == 11))		/* They have the raised form */
        Val += DoubledLeadingTonePenalty;

      /* similarly for motion to a tritone */
      if ((MotionType(LastCp,Cp,Other1,Other0) == DirectMotion) && (Int0 == Tritone))
        Val += InnerVoicesInDirectToTritonePenalty;

      /* look for a common diminished fourth (when a raised leading tone is in 
       * the bass, a "major third" above it is actually a diminished fourth.
       * Similarly, an augmented fifth can be formed in other cases 
       */
      if ((ActPitch == 3) && ((Other0 % 12) == 11)) Val += AugmentedIntervalPenalty;

      /* try to encourage voices not to move in parallel too much */
      if (MotionType(LastCp,Cp,Other1,Other0) != ContraryMotion) Val += NotContraryToOthersPenalty;
    }

  /* check for doubled third */
  if (IntervalsWithBass[3]>1) Val += ThirdDoubledPenalty;

  /* check for doubled sixth */
  if ((IntervalsWithBass[3] == 0) && (IntervalsWithBass[6]>1)) Val += DoubledSixthPenalty;
  
  /* check for too many voices at octaves */
  if (IntervalsWithBass[0]>2) Val += TripledBassPenalty;

  /* check for doubled fifth */
  if (IntervalsWithBass[5]>1) Val += DoubledFifthPenalty;

  /* check that chord contains at least one third or sixth */
  if ((v == NumParts) && ((!(LastNote(Cn,v))) && ((IntervalsWithBass[3] == 0) && (IntervalsWithBass[6] == 0))))
    Val += NotTriadPenalty;
  
  /* discourage all voices from skipping at once */
  if ((v == NumParts) && AllSkip) Val += AllVoicesSkipPenalty;
  
  /* except in 5th species, disallow 6-5 chords altogether */
  if ((IntervalsWithBass[5]>0) && ((IntervalsWithBass[6]>0) && (Species != 5))) Val += SixFiveChordPenalty;
  return(Val);
}

int Check(int Cn, int Cp, int v, int NumParts, int Species, int CurLim)
{
  int Val,k,Interval,IntClass,Pitch,LastIntClass,MelInt,LastMelInt,Other0,Other1,Other2;
  int Cross,SameDir,WeHaveARealLeadingTone,LastPitch,totalJump,LastCp,LastCp2,LastCp3,LastCp4;
  if (v == 1)
    {
      Other0=Cantus(Cn,v);
      Other1=Cantus(Cn-1,v);
      if (Cn>2) {Other2=Cantus(Cn-2,v);}
    }
  else
    {
      Other0=Bass(Cn,v);
      Other1=Bass(Cn-1,v);
      if (Cn>2) {Other2=Bass(Cn-2,v);}
    }
  Val=0;
  LastCp=Us(Cn-1,v);
  LastCp2=0; LastCp3=0; LastCp4=0;
  Interval=(Cp-Other0);
  IntClass=(ABS(Interval)) % 12;
  MelInt=(Cp-LastCp);
  Pitch=(Cp % 12);

  /* melody must stay in range */
  if (OutOfRange(Cp+BasePitch)) Val += OutOfRangePenalty;

  /* extremes of range are also bad (to be avoided) */
  if (ExtremeRange(Cp+BasePitch)) Val += ExtremeRangePenalty;

  /* two part with ctrpt below cantus -- keep it below */
  if ((NumParts == 1) && ((Us(1,v) < Cantus(1,v)) && (Interval > Unison))) Val += CrossAboveCantusPenalty;

  /* Chromatically altered notes are accepted only at the cadence.  Other alterations (such as ficta) will be handled later) */
  if (!(NextToLastNote(Cn,v)))
    {
      if (Species != 2)
	{
	  if (!(InMode(Pitch,Mode))) Val += OutOfModePenalty;
	}
      else
	{
	  if ((Cn != TotalNotes[v]-2) || ((Mode != Aeolian) || ((Cp <= Other0) || (IntClass != Fifth))))
	    {
              if (!(InMode(Pitch,Mode))) Val += OutOfModePenalty;
	    }
	}
    }
  else
    {
      WeHaveARealLeadingTone = ((Pitch == 11) || ((Pitch == 10) && (Mode == Phrygian)));
      if (WeHaveARealLeadingTone)
	{
	  if (Doubled(Pitch,Cn,v)) Val += DoubledLeadingTonePenalty;
	}
      else
	{
	  if (Pitch == 10) Val += BadCadencePenalty;
	  else
	    {
	      if (!(InMode(Pitch,Mode))) Val += OutOfModePenalty;
	      else
		{
		  if (v == NumParts)
		    {
		      if ((!(Doubled(11,Cn,v))) && (!(Doubled(10,Cn,v)))) Val += NoLeadingTonePenalty;
		    }
		}
	    }
	}
    }
  if (Val >= CurLim) return(Val);
  if (Cn>2)
    {
      LastCp2=Us(Cn-2,v);
      if (Cn>3)
	{
	  LastCp3=Us(Cn-3,v);
	  if (Cn>4) LastCp4=Us(Cn-4,v);
	}
      LastMelInt=(LastCp-LastCp2);
      SameDir=((MelInt*LastMelInt) >= 0);
    }
  if (Cn>1) {LastIntClass=((ABS(LastCp-Other1)) % 12);}
  if (ADissonance(IntClass,Cn,Cp,v,Species)) Val += DissonancePenalty;
  if (Val >= CurLim) return(Val);
  Val += SpecialSpeciesCheck(Cn,Cp,v,Other0,Other1,Other2,NumParts,Species,MelInt,Interval,IntClass,LastIntClass,Pitch,LastMelInt,CurLim);
  if (v>1) Val += OtherVoiceCheck(Cn,Cp,v,NumParts,Species,CurLim);
  if (FirstNote(Cn,v)) return(Val);
  /* no further rules apply to first note */
  if (Val >= CurLim) return(Val);

  /* direct motion to perfect consonances considered harmful */
  if ((!(LastNote(Cn,v))) || (NumParts == 1))
    {
      if (DirectMotionToPerfectConsonance(LastCp,Cp,Other1,Other0))
	{
	  if (IntClass == Unison) Val += DirectToOctavePenalty;
	  else Val += DirectToFifthPenalty;
	}
    }

  /* check for more blatant examples of the same error */
  if ((IntClass == Fifth) && (LastIntClass == Fifth)) Val += ParallelFifthPenalty;
  if ((IntClass == Unison) && (LastIntClass == Unison)) Val += ParallelUnisonPenalty;
  if (Val >= CurLim) return(Val);

  if ((Cn>1) && ((Species == 1) && ((NumParts == 1) && ((IntClass == LastIntClass) && (MelInt == Unison)))))
    Val += NoMotionAgainstOctavePenalty;

  /* certain melodic intervals are disallowed */
  if (BadMelody(MelInt)) Val += BadMelodyPenalty;
  if (Val >= CurLim) return(Val);

  /* must end on unison or octave in two parts, fifth and major third allowed in 3 and 4 part writing */
  if ((LastNote(Cn,v)) && (IntClass != Unison))
    {
      if ((NumParts == 1) || (Interval<0)) Val += EndOnPerfectPenalty;
      else
	{
          if ((IntClass != Fifth) && (IntClass != MajorThird)) Val += EndOnPerfectPenalty;
	}
    }

  /* penalize direct motion any kind (contrary motion is better) */
  if (MotionType(LastCp,Cp,Other1,Other0) == DirectMotion)
    {
      Val += DirectMotionPenalty;
      if (IntClass == Tritone) Val += DirectToFifthPenalty;
    }

  /* penalize compound intervals (close position is favored) */
  if ((ABS(Interval))>Octave) Val += CompoundPenalty;

  /* penalize consecutive skips in the same direction */
  if ((Cn>2) && (ConsecutiveSkipsInSameDirection(LastCp2,LastCp,Cp)))
    {
      Val += TwoSkipsPenalty;
      totalJump=ABS(Cp-LastCp2);

      /* do not let these skips traverse more than an octave, nor a seventh */
      if ((totalJump > MajorSixth) && (totalJump < Octave)) Val += TwoSkipsNotInTriadPenalty;
    }

  /* penalize a skip to an octave */
  if ((IntClass == Unison) && ((ASkip(MelInt)) || (ASkip(Other0-Other1)))) Val += SkipTo8vePenalty;

  /* do not skip from a unison (not a very important rule) */
  if ((Other1 == LastCp) && (ASkip(MelInt))) Val += SkipFromUnisonPenalty;

  /* penalize skips followed or preceded by motion in same direction */
  if ((Cn>2) && ((ASkip(MelInt)) && SameDir))
    {
      /* especially penalize fifths, sixths, and octaves of this sort */
      if ((ABS(MelInt)) < Fifth) Val += SkipPrecededBySameDirectionPenalty;
      else
	{
          if (((ABS(MelInt)) == Fifth) || ((ABS(MelInt)) == Octave)) 
	    Val += FifthPrecededBySameDirectionPenalty;
	  else Val += SixthPrecededBySameDirectionPenalty;
	}
    }
  if ((Cn>2) && ((ASkip(LastMelInt)) && SameDir))
    {
      if ((ABS(LastMelInt)) < Fifth) Val += SkipFollowedBySameDirectionPenalty;
      else
	{
          if (((ABS(LastMelInt)) == Fifth) || ((ABS(LastMelInt)) == Octave))
            Val += FifthFollowedBySameDirectionPenalty;
          else Val += SixthFollowedBySameDirectionPenalty;
	}
    }

  /* too many skips in a row -- favor a mix of steps and skips */
    if ((Cn>4) && ((ASkip(MelInt)) && ((ASkip(LastMelInt)) && (ASkip(LastCp2-LastCp3))))) Val += MelodicBoredomPenalty;

  /* avoid tritones melodically */
  if ((Cn>4) && (((ABS(Cp-LastCp2)) == Tritone) || (((ABS(Cp-LastCp3)) == Tritone) || ((ABS(Cp-LastCp4)) == Tritone))))
    Val += MelodicTritonePenalty;

  /* do not allow movement from a tenth to an octave by contrary motion */
  if ((Species != 5) && (NumParts == 1))
    {
      if (ATenth(Other1-LastCp) && (AnOctave(Interval))) Val += TenthToOctavePenalty;
    }

  /* more range checks -- did we go over an octave recently */
  if ((Cn>2) && ((ABS(Cp-LastCp2)) > Octave)) Val += OverOctavePenalty;

  /* same for a twelfth */
  if (((Cn>30) || (Species != 5)) && (TotalRange(Cn,Cp,v) > (Octave+Fifth))) Val += OverTwelfthPenalty;
  if (Val >= CurLim) return(Val);

  /* slightly penalize repeated notes */
  if ((Cn>3) && ((Cp == LastCp2) && (LastCp == LastCp3))) Val += TwoRepeatedNotesPenalty;
  if ((Cn>5) && ((Cp == LastCp3) && ((LastCp == LastCp4) && (LastCp2 == Us(Cn-5,v))))) Val += ThreeRepeatedNotesPenalty;
  if ((Cn>6) && ((Cp == LastCp4) && ((LastCp == Us(Cn-5,v)) && (LastCp2 == Us(Cn-6,v))))) Val += (ThreeRepeatedNotesPenalty-1);
  if ((Cn>7) && ((Cp == LastCp4) && ((LastCp == Us(Cn-5,v)) &&
       ((LastCp2 == Us(Cn-6,v)) && (LastCp3 == Us(Cn-7,v)))))) Val += FourRepeatedNotesPenalty;
  if ((Cn>8) && ((Cp == Us(Cn-5,v)) && ((LastCp == Us(Cn-6,v)) &&
      ((LastCp2 == Us(Cn-7,v)) && (LastCp3 == Us(Cn-8,v)))))) Val += FourRepeatedNotesPenalty;
  if (LastNote(Cn,v))
    {
      LastPitch=(LastCp % 12);
      if (((LastPitch == 11) || ((LastPitch == 10) && (Mode == Phrygian))) && (Pitch != 0)) Val += UnresolvedLeadingTonePenalty;
    }
  if (Val >= CurLim) return(Val);

  /* an imperfect consonance is better than a perfect consonance */
  if (PerfectConsonance[IntClass]) Val += PerfectConsonancePenalty;

  /* no unisons allowed within counterpoint unless more than 2 parts */
  if ((NumParts == 1) && (Interval == Unison)) Val += UnisonPenalty;
  if (Val >= CurLim) return(Val);

  /* seek variety by avoiding pitch repetitions */
  Val += (PitchRepeats(Cn,Cp,v)>>1);

  /* penalize octave leaps a little */
  if (AnOctave(MelInt)) Val += OctaveLeapPenalty;

  /* similarly for minor sixth leaps */
  if (MelInt == MinorSixth) Val += SixthLeapPenalty;

  /* penalize upper neighbor notes slightly (also lower neighbors) */
  if ((Cn>2) && ((MelInt<0) && ((AStep(MelInt)) && (Cp == LastCp2)))) Val += UpperNeighborPenalty;
  if ((Cn>2) && ((MelInt>0) && ((AStep(MelInt)) && (Cp == LastCp2)))) Val += LowerNeighborPenalty;

  /* do not allow normal leading tone to precede raised leading tone */
  /* also check here for augmented fifths and diminished fourths */
  if ((!(InMode(Pitch,Mode))) && ((MelInt == MinorSecond) || ((MelInt == MinorSixth) || (MelInt == (-MajorThird))))) Val += OutOfModePenalty;   

  /* slightly frown upon leap back in the opposite direction */
  if ((Cn>2) && ((ASkip(MelInt)) && ((ASkip(LastMelInt)) && (!(SameDir)))))
    {
      Val += (MAX(0,((ABS(MelInt)+ABS(LastMelInt))-8)));
      if ((Cn>3) && (ASkip(LastCp2-LastCp3))) Val += ThreeSkipsPenalty;
    }

  /* try to approach cadential passages by step */
  if ((NumParts == 1) && ((Cn >= (TotalNotes[v]-4)) && ((ABS(MelInt)) > 4))) Val += LeapAtCadencePenalty;

  /* check for entangled voices */
  Cross=0;
  if (NumParts == 1)
    {
      for (k=4;k<=Cn;k++)
	{
          if ((Us(k,v)-Cantus(k,v))*(Us(k-1,v)-Cantus(k-1,v)) < 0) Cross++;
	}
    }
  if (Cross > 0) Val += (MAX(0,((Cross-2)*3)));
  
  /* don't repeat note on upbeat */
  if (UpBeat(Cn,v) && (MelInt == Unison)) Val += RepetitionOnUpbeatPenalty;
 
  /* avoid tritones near Lydian cadence */
  if ((Mode == Lydian) && ((Cn>(TotalNotes[v]-4)) && (Pitch == 6))) Val += LydianCadentialTritonePenalty;

  /* various miscellaneous checks.  More elaborate dissonance resolution and cadential formula checks will be given under "Species definition" */
  if ((Species != 1) && (DownBeat(Cn,v)))
    {
      if (Species<4)
	{
	  if ((MelInt == Unison) && (!(LastNote(Cn,v)))) Val += UnisonDownbeatPenalty;
	  /* check for dissonance that doesn't fill a third as a passing tone */
	  if ((Dissonance[LastIntClass]) && ((!(AStep(MelInt))) || (!(SameDir)))) Val += DissonanceNotFillingThirdPenalty;
	}

      /* check for Direct 8ve or 5 where the intervening interval is less than a fourth */
      if ((DirectMotionToPerfectConsonance(LastCp2,Cp,Other2,Other0)) && ((ABS(LastMelInt)) < Fourth))
	Val += DirectPerfectOnDownbeatPenalty;
    }

  /* check for tritone with cantus or bass */
  if (IntClass == Tritone) Val += VerticalTritonePenalty;

  /* check for melodic interval variety */
  if ((Cn>10) && (TooMuchOfInterval(Cn,Cp,v))) Val += MelodicBoredomPenalty;

  return(Val);
}


int BestFit[MostNotes][MostVoices];
int BestFit1[MostNotes][MostVoices]; /* next-to-best fits (for testing) */
int BestFit2[MostNotes][MostVoices];
int Fits[3];
int BestFitPenalty,MaxPenalty,Branches,AllDone;
float PenaltyRatio;

#define NumFields 16
#define Field (MostVoices+1)
#define EndF (Field*NumFields)

int SaveIndx(int indx, int *Sp)
{
  int i;
  /* if INDX is less than current NUMFIELD-th worst,
   * find its position in SP, insert space for its
   * data, and return a pointer to the block.  The
   * blocks are stored "backwards" for (SAIL's) ARRBLT 
   */
  i=EndF;
  while ((i>=0) && (Sp[i]<=indx)) {i-=Field;}
  if (i>0)	/* 0 is the end of the list.  If i>0 then we insert INDX */
    {
      ARRBLT(Sp,(int *)(Sp+Field),i);
      Sp[i]=indx;
      /* SP[i]=penalty for block starting at I.  SP[i-1]=index into
       * melodic interval array for voice 1, SP[i-2] for voice 2 and
       * so on.  The searcher starts at SP[EndF] and works backwards
       * through the stored continuations as it searches for a satisfactory
       * overall solution 
       */
    }
  return(i);
}

void SaveResults(int CurrentPenalty, int Penalty, int v1, int Species)
{
  int i,LastPitch,v,Cn,k,Pitch,done;
  for (v=1;v<=v1;v++)
    {
      /* check all voices for raised leading tone */
      Cn=TotalNotes[v];
      LastPitch=(Us(Cn-1,v) % 12);	/* must be raised if any are */
      if (!(InMode(LastPitch,Mode)))    /* it is a raised leading tone */
	{
	  k=2;
	  while (1)                     /* exit via break */
	    {
	      /* look backwards through voice's notes */
	      if (k >= (Cn-1)) break;	                /* ran off start!! */
              Pitch=(Us(Cn-k,v) % 12);	                /* current pitch */
              if (((Pitch<8) && (Pitch != 0)) ||	/* not 6-7-1 scale degree anymore */
                  (ASkip(Us(Cn-k+1,v)-Us(Cn-k,v))))     /* skip breaks drive to cadence */
		break;
              Pitch=ABS(Us(Cn-k,v)-Us(Cn-k-1,v));       /* interval with raised leading tone */
              if ((Pitch == Fourth) || ((Pitch == Fifth) || ((Pitch == Unison) || (Pitch == Octave)))) break;
	      /* don't create illegal melody */
	      done = 0;
	      i=0;
	      while (i<=v1)             /* do others have unraised form? */
		{
		  if ((i != v) && (((Other(Cn-k,v,i)) % 12) == 11)) 
		    {
		      done = 1;
		      break;
		    }
		  i++;
		}
	      if (done) break;
	      if (((Us(Cn-1,v)-Us(Cn-k,v)) == MinorThird) || ((Us(Cn-1,v)-Us(Cn-k,v)) == MinorSecond))
                SetUs(Cn-k,Us(Cn-k,v)+1,v);            /* raise it and maybe 6th degree too */
	      k++;
	    }
	}
    }
  BestFitPenalty=CurrentPenalty+Penalty;
  MaxPenalty=MIN(BestFitPenalty*PenaltyRatio,MaxPenalty);
/*  AllDone=1; */
  Fits[2]=Fits[1]; Fits[1]=Fits[0]; Fits[0]=BestFitPenalty;
  for (v=1;v<=v1;v++)
    {
      for (i=1;i<=TotalNotes[v];i++)
	{
 	  BestFit2[i][v]=BestFit1[i][v];       
 	  BestFit1[i][v]=BestFit[i][v];        
	  BestFit[i][v]=Ctrpt[i][v]+BasePitch; 
	}
    }
#ifndef CM
  printf("\n [%d] ",BestFitPenalty);
  for (v=1;v<=v1;v++)
    {
      for (i=1;i<=TotalNotes[v];i++)
	{
	  printf("%d ",BestFit[i][v]);
	}
      printf("\n");
    }
#endif
}

int Indx[17] = {0,1,-1,2,-2,3,-3,0,4,-4,5,7,-5,8,12,-7,-12};

int Look(int CurPen, int CurVoice, int NumParts, int Species, int Lim, int *Pens, int *Is, int *CurNotes)
{
  int penalty,Pit,i,x,tmp1,NewLim;
  NewLim=Lim;
  for (Is[CurVoice]=1;Is[CurVoice]<=16;Is[CurVoice]++)
    {
      Pit=Indx[Is[CurVoice]]+Ctrpt[CurNotes[CurVoice]-1][CurVoice];
      if (CurVoice == NumParts) tmp1=Species; else tmp1=1;
      penalty=CurPen+Check(CurNotes[CurVoice],Pit,CurVoice,NumParts,tmp1,NewLim);
      SetUs(CurNotes[CurVoice],Pit,CurVoice);
      if (penalty<NewLim)
	{
          if (CurVoice<NumParts)
	    {
	      i=(CurVoice+1);
	      while (i<=NumParts)
		{
		  if (CurNotes[i] != 0) break;
		  i++;
		}
              if (i <= NumParts)	/* there is another voice needing a note */
                {
		  NewLim=Look(penalty,i,NumParts,Species,NewLim,Pens,Is,CurNotes);
		}
	    }
	  else
	    {
	      x=SaveIndx(penalty,Pens);
	      if (x>0)
		{
		  for (i=1;i<=NumParts;i++) Pens[x-i]=Is[i];
		}
	      else NewLim=MIN(NewLim,penalty);
	    }
	}
    }
  return(NewLim);
}

void BestFitFirst(int CurTime, int CurrentPenalty, int NumParts, int Species, int BrLim)
{
  int i,j,CurMin,Lim,ChoiceIndex,NextTime,OurTime;
  int *Pens,*Is,*CurNotes;
  if ((AllDone) || (CurrentPenalty>MaxPenalty)) return;

  Branches++;
  Pens=(int *)calloc(1+(Field*NumFields),sizeof(int));
  Is=(int *)calloc(1+NumParts,sizeof(int));
  CurNotes=(int *)calloc(1+MostVoices,sizeof(int));

  ChoiceIndex=EndF;
  AllDone=0;
  for (i=0;i<=(Field*NumFields);i++) Pens[i]=infinity;
  for (i=0;i<=NumParts;i++) Is[i]=0;
  for (i=0;i<=MostVoices;i++) CurNotes[i]=0;

  if (Branches == BrLim) {MaxPenalty = MaxPenalty*PenaltyRatio; Branches=0;}

  CurMin=infinity;
  Lim=BestFitPenalty-CurrentPenalty;
  NextTime=infinity;
  for (i=0;i<=NumParts;i++)
    {
      OurTime=Onset[VIndex(CurTime,i)+1][i];
      if (OurTime != 0) NextTime=MIN(NextTime,OurTime);
    }
  for (i=1;i<=NumParts;i++)
    {
      j=VIndex(NextTime,i);
      if (Onset[j][i] == NextTime) CurNotes[i]=j;
    }
  i=1;
  while (i<=NumParts)
    {
      if (CurNotes[i] != 0) break;
      i++;
    }
  Lim=Look(0,i,NumParts,Species,Lim,Pens,Is,CurNotes);

  CurMin=Pens[ChoiceIndex];
  if (CurMin < infinity)
    {
      AllDone=0;
      while (!(AllDone))
	{
	  if (CurTime<TotalTime)
	    {
	      if ((CurMin+CurrentPenalty) >= MaxPenalty) break;
	    }
	  else
	    {
	      if ((CurMin+CurrentPenalty) >= BestFitPenalty) break;
	    }
	  
	  for (i=1;i<=NumParts;i++)
	    {
	      if (CurNotes[i] != 0) SetUs(CurNotes[i],Indx[Pens[ChoiceIndex-i]]+Us(CurNotes[i]-1,i),i);
	    }
	  if (NextTime<TotalTime)
	    BestFitFirst(NextTime,CurrentPenalty+CurMin,NumParts,Species,BrLim);
	  else
	    SaveResults(CurrentPenalty,CurMin,NumParts,Species);
	  
	  ChoiceIndex=ChoiceIndex-Field;
	  if (ChoiceIndex <= 0) break;
	  CurMin=Pens[ChoiceIndex];
	  if (CurMin == infinity) break;
	  if (CurTime == 0) MaxPenalty=(BestFitPenalty*PenaltyRatio);
	}
    }

  free(CurNotes);
  free(Is);
  free(Pens);
}

int RhyPat[11][9],RhyNotes[11];

void FillRhyPat()
{
  RhyPat[0][1]=WholeNote;
  RhyNotes[0]=1;
  RhyPat[1][0]=0; RhyPat[1][1]=HalfNote; RhyPat[1][2]=HalfNote;
  RhyNotes[1]=2;
  RhyPat[2][0]=0; RhyPat[2][1]=HalfNote; RhyPat[2][2]=QuarterNote; RhyPat[2][3]=QuarterNote; 
  RhyNotes[2]=3;
  RhyPat[3][0]=0; RhyPat[3][1]=QuarterNote; RhyPat[3][2]=QuarterNote; RhyPat[3][3]=QuarterNote; RhyPat[3][4]=QuarterNote; 
  RhyNotes[3]=4;
  RhyPat[4][0]=0; RhyPat[4][1]=QuarterNote; RhyPat[4][2]=QuarterNote; RhyPat[4][3]=HalfNote; 
  RhyNotes[4]=3;
  RhyPat[5][0]=0; RhyPat[5][1]=QuarterNote; RhyPat[5][2]=EighthNote; RhyPat[5][3]=EighthNote; RhyPat[5][4]=HalfNote;
  RhyNotes[5]=4;
  RhyPat[6][0]=0; RhyPat[6][1]=QuarterNote; RhyPat[6][2]=EighthNote; RhyPat[6][3]=EighthNote; RhyPat[6][4]=QuarterNote; RhyPat[6][5]=QuarterNote;
  RhyNotes[6]=5;
  RhyPat[7][0]=0; RhyPat[7][1]=HalfNote; RhyPat[7][2]=QuarterNote; RhyPat[7][3]=EighthNote; RhyPat[7][4]=EighthNote;
  RhyNotes[7]=4;
  RhyPat[8][0]=0; RhyPat[8][1]=QuarterNote; RhyPat[8][2]=EighthNote; RhyPat[8][3]=EighthNote; 
  RhyPat[8][4]=QuarterNote; RhyPat[8][5]=EighthNote; RhyPat[8][6]=EighthNote; 
  RhyNotes[8]=6;
  RhyPat[9][0]=0; RhyPat[9][1]=QuarterNote; RhyPat[9][2]=QuarterNote; RhyPat[9][3]=QuarterNote; 
  RhyPat[9][4]=EighthNote; RhyPat[9][5]=EighthNote; 
  RhyNotes[9]=5;
  RhyPat[10][1]=WholeNote;
  RhyNotes[10]=1;
}

static long randx = 1;
#define inverse_rscl .000061035156

float RANDOM(float amp)
{
  int i = ((randx = randx*1103515245 + 12345)>>16) & 077777;
  return(amp * (((float)i)*inverse_rscl));
}

void UsedRhy(int n) {RhyPat[n][0]=RhyPat[n][0]+1;}
int CurRhy(int n) {return(RhyPat[n][0]);}
void CleanRhy() {int i; for (i=1;i<10;i++) RhyPat[i][0]=0;}
int GoodRhy()
{
  int i;
  i=(int)(RANDOM(10.0));
  if (CurRhy(i) >  CurRhy(MAX(1,(i-1)))) return(MAX(1,(i-1)));
  if (CurRhy(i) <= CurRhy(MIN(9,(i+1)))) return(MIN(9,(i+1)));
  return(i);
}

void AnySpecies(int OurMode, int *StartPitches, int CurV, int CantusFirmusLength, int Species)
{
  int i,j,k,m,v,OldSpecies,CurrentMode,BrLim;
  for (i=0;i<MostNotes;i++)
    for (j=1;j<MostVoices;j++)
      {
	BestFit[i][j]=0;
	Ctrpt[i][j]=0;
      }
  PenaltyRatio=(1.0-(Species*CurV*.01));
  BrLim=(50*(6-CurV)*(6-Species));
  CurrentMode=OurMode;
  Mode=OurMode;
  TotalTime=((CantusFirmusLength-1)*8);
  TotalNotes[0]=CantusFirmusLength;
  BasePitch=((Ctrpt[CantusFirmusLength][0]) % 12);
  BestFitPenalty=infinity;
  MaxPenalty=infinity;
  AllDone=0;
  Branches=0;

  for (i=1;i<=CantusFirmusLength;i++) 
    {
      Ctrpt[i][0] -= BasePitch;
      Dur[i][0] = WholeNote;
      Onset[i][0] = ((i-1)*8);
    }
  OldSpecies=Species;
  for (v=1;v<=CurV;v++)
    {
      if (v != CurV) Species=1;
      else Species = OldSpecies;
      if (Species == 1)
	{
	  TotalNotes[v]=CantusFirmusLength;
	  for (i=1;i<CantusFirmusLength;i++) Dur[i][v] = WholeNote;
	}
      else
	if (Species == 2)
	  {
	    TotalNotes[v]=(CantusFirmusLength*2)-1;
	    for (i=1;i<TotalNotes[v];i++) Dur[i][v] = HalfNote;
	  }
      else
	if (Species == 3)
	  {
	    TotalNotes[v]=(CantusFirmusLength*4)-3;
	    for (i=1;i<TotalNotes[v];i++) Dur[i][v] = QuarterNote;
	  }
      else
	if (Species == 4)
	  {
	    TotalNotes[v]=(CantusFirmusLength*2)-1;
	    for (i=1;i<TotalNotes[v];i++) Dur[i][v] = HalfNote;
	  }
      else
	{
	  CleanRhy();
	  m=0;
	  for (i=1;i<CantusFirmusLength;i++)
	    {
	      j=GoodRhy();
	      UsedRhy(j);
	      for (k=1;k<=(RhyNotes[j]);k++) Dur[k+m][v]=RhyPat[j][k];
	      m += RhyNotes[j];
	    }
	  TotalNotes[v]=(m+1);
	}
      Dur[TotalNotes[v]][v]=WholeNote;
      Onset[1][v]=0;
      for (k=2;k<=TotalNotes[v];k++) Onset[k][v]=(Onset[k-1][v]+Dur[k-1][v]);
      Ctrpt[1][v]=(StartPitches[v-1]-BasePitch);
    }
  if (CurV == 1) MaxPenalty=(2*RealBad); else MaxPenalty=infinity;
  BestFitFirst(0,0,CurV,Species,BrLim);
}
	
void fillCantus(int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7, int c8, int c9, int c10, int c11, int c12, int c13, int c14)
{
  Ctrpt[1][0]=c0; Ctrpt[2][0]=c1; Ctrpt[3][0]=c2; Ctrpt[4][0]=c3; Ctrpt[5][0]=c4; Ctrpt[6][0]=c5; Ctrpt[7][0]=c6;
  Ctrpt[8][0]=c7; Ctrpt[9][0]=c8; Ctrpt[10][0]=c9; Ctrpt[11][0]=c10; Ctrpt[12][0]=c11; Ctrpt[13][0]=c12;
  Ctrpt[14][0]=c13; Ctrpt[15][0]=c14;
}

#ifdef CM
int rhyfilled =0;

void fux(int mode, int species, int voices, int cantuslen, int *voicebegs, int *cantus)
{
  int i;
  if (rhyfilled == 0)
    {
      rhyfilled = 1;
      FillRhyPat();
    }
  for (i=1;i<=cantuslen;i++) Ctrpt[i][0] = cantus[i-1];
  for (i=0;i<3;i++) Fits[i]=0;
  AnySpecies(mode,voicebegs,voices,cantuslen,species);
}

void winners(int v1, int *data, int *best, int *best1, int *best2, int *durs)
{
  int i,v,k;
  for (v=1;v<=v1;v++)
    {
      k=(v*MostNotes)+1;
      for (i=1;i<=TotalNotes[v];i++,k++)
	{
	  best[k]=BestFit[i][v];
	  best1[k]=BestFit1[i][v];
	  best2[k]=BestFit2[i][v];
	  durs[k]=Dur[i][v];
	}
    }
  data[0]=Fits[0];
  data[1]=Fits[1];
  data[2]=Fits[2];
  for (v=1;v<=v1;v++) data[2+v]=TotalNotes[v];
}

#else

int vbs[MostVoices];

main()
{
  FillRhyPat();

#if EXS
  fillCantus(50,53,52,50,55,53,57,55,53,52,50,0,0,0,0); 
  vbs[0]=57; vbs[1]=62;
  AnySpecies(Dorian,vbs,1,11,1);            /* 57 62 -- 38,45,57,62,69,53,50 */
  vbs[0]=38;
  AnySpecies(Dorian,vbs,1,11,1);            /* 38 57 */

  fillCantus(52,48,50,48,45,57,55,52,53,52,0,0,0,0,0);
  vbs[0]=59;
  AnySpecies(Phrygian,vbs,1,10,1);          /* 59 64 -- 28,59,64,55,71,40,55 */
  vbs[0]=40;
  AnySpecies(Phrygian,vbs,1,10,1);          /* 40 59 */

  fillCantus(53,55,57,53,50,52,53,60,57,53,55,53,0,0,0);
  vbs[0]=65;
  AnySpecies(Lydian,vbs,1,12,1);            /* 65 60 -- 41,60,65,72,57,48,69 */
  vbs[0]=41;
  AnySpecies(Lydian,vbs,1,12,1);            /* 41 60 */

  fillCantus(43,48,47,43,48,52,50,55,52,48,50,47,45,43,0);
  vbs[0]=55;
  AnySpecies(Mixolydian,vbs,1,14,1);        /* 55 62 -- 31,55,62,50,59,67,71 */
  vbs[0]=43;
  AnySpecies(Mixolydian,vbs,1,14,1);        /* 31 55 */

  fillCantus(45,48,47,50,48,52,53,52,50,48,47,45,0,0,0);
  vbs[0]=57;
  AnySpecies(Aeolian,vbs,1,12,1);           /* 57 64 -- 33,64,52,57,69,40,60 */
  vbs[0]=45;
  AnySpecies(Aeolian,vbs,1,12,1);           /* 45 64 */

  fillCantus(50,53,52,50,55,53,57,55,53,52,50,0,0,0,0);
  vbs[0]=57; vbs[1]=62;
  AnySpecies(Dorian,vbs,2,11,1);            /* 57 62 -- 38,45,57,62,69,53,50 */
#endif

  fillCantus(50,53,52,50,55,53,57,55,53,52,50,0,0,0,0);
  vbs[0]=38; vbs[1]=57; vbs[2]=62;
  AnySpecies(Dorian,vbs,1,11,1);            /* 57 62 -- 38,45,57,62,69,53,50 */

}
#endif
