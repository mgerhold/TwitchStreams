using NUnit.Framework;
using UnitTestsStream;

namespace Tests
{
    public class Vector3Tests
    {
        [Test]
        public void CreateDefaultConstructedVector_CheckIfValuesAreZero()
        {
            Vector3 a = new Vector3();

            // entweder diese Schreibweise:
            Assert.AreEqual(0.0, a.x, "x coordinate should have been 0");

            // oder diese:
            Assert.That(a.y, Is.EqualTo(0.0), "y coordinate should have been 0");
            Assert.That(a.z, Is.EqualTo(0.0), "z coordinate should have been 0");
        }
    }
}