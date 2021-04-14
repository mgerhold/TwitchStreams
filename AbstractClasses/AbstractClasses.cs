using System;
using System.Collections.Generic;

public abstract class Tier {
	public abstract void GibLaut();
}

public class Hund : Tier {
	public override void GibLaut() {
		Console.WriteLine("Wau!");
	}
}

public class Katze : Tier {
	public override void GibLaut() {
		Console.WriteLine("Miau!");
	}
}

public class Program
{	
	public static void Main()
	{
		Tier einTier = new Hund();
		einTier.GibLaut(); // virtual dispatch (virtual table = v-table)
		
		Console.WriteLine("===============================");
		
		// Tier tier = new Tier(); <- nicht möglich, da man keine abstrakte Klasse instanziieren kann
		List<Tier> tiere = new List<Tier>();
		tiere.Add(new Hund());
		tiere.Add(new Hund());
		tiere.Add(new Katze());
		tiere.Add(new Hund());
		
		foreach (Tier tier in tiere) {
			tier.GibLaut(); // virtual dispatch (virtual table = v-table)
		}
		
		// ohne Vererbungshierarchie
		List<Hund> hunde = new List<Hund>();
		List<Katze> katzen = new List<Katze>();
		hunde.Add(new Hund());
		hunde.Add(new Hund());		
		katzen.Add(new Katze());
		
		foreach (Hund hund in hunde) {
			hund.GibLaut(); // kein virtual dispatch
		}
		
		foreach (Katze katze in katzen) {
			katze.GibLaut(); // kein virtual dispatch
		}
	}
	
}