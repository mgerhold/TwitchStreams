using System;

// Vererbung (Inheritance) beschreibt ein "ist ein"-Verhältnis

public abstract class Weapon {
	public string Name { get; set; }
	public int AttackDamage { get => CalculateAttackDamage(); }	
	
	protected abstract int CalculateAttackDamage();
}

public abstract class MeleeWeapon : Weapon {
	public abstract void Attack();
}

public abstract class RangedWeapon : Weapon {
	public abstract void RangedAttack();
}

public class Bow : RangedWeapon {
	public override void RangedAttack() {
		
	}
	protected override int CalculateAttackDamage() {
		return 6;
	}	
}

public class Sword : MeleeWeapon {
	public override void Attack() {
		
	}
	protected override int CalculateAttackDamage() {
		return 12;
	}
}

public class Player {
	public Weapon[] Weapons { get; set; } = { null, null };
	
	public void MeleeAttack() {
		foreach (var weapon in Weapons) {
			if (weapon is MeleeWeapon) {
				((MeleeWeapon)weapon).Attack();
				Console.WriteLine($"player is attacking with weapon of type {weapon.GetType()}");
				break;
			}
		}
	}
	
	private int hitPoints;
	public int HitPoints {
		get => hitPoints;
		set { hitPoints = Math.Max(0, value); }
	}
	
	/*private int hitPoints;
	
	public void SetHitPoints(int hitPoints) {
		hitPoints = Math.Max(0, value);
	}
	
	public int GetHitPoints() {
		return hitPoints;
	}*/
}

public class Program
{
	public static void Main()
	{
		Player player = new Player();
		player.Weapons[0] = new Sword();
		player.Weapons[0].Name = "Mighty Sword";
		player.Weapons[1] = new Bow();
		player.Weapons[1].Name = "Bow of Odin";
		
		player.MeleeAttack();
	}
}