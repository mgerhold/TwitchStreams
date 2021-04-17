using System;
using System.Collections.Generic;
using System.Reflection;

public abstract class MonoBehaviour {
}

public class VideoModeManager : MonoBehaviour {
	private void Start() {
		Console.WriteLine("VideoModeManager started");
	}
}

public class DiceManager : MonoBehaviour {
	// ...
}

public class JackCimberlyClass : MonoBehaviour {
	private void Start() {
		Console.WriteLine("Jack Cimberly is da man!");
	}
}

public class Program
{	
	public static void Main()
	{
		List<MonoBehaviour> scripts = new List<MonoBehaviour>();
		scripts.Add(new VideoModeManager());
		scripts.Add(new DiceManager());
		scripts.Add(new DiceManager());
		scripts.Add(new JackCimberlyClass());
		scripts.Add(new DiceManager());
		scripts.Add(new JackCimberlyClass());
		
		foreach (MonoBehaviour script in scripts) {
			MethodInfo[] methodInfos = script.GetType().GetMethods(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);			
			foreach (var methodInfo in methodInfos) {
				if (methodInfo.ToString() == "Void Start()") {
					methodInfo.Invoke(script, null);
				}
			}
		}
		
	}
	
}