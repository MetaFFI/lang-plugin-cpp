"""
Plugin hooks for the MetaFFI C++ plugin.

Invoked by the CLI installer:
  python plugin_hooks.py --check-prerequisites
  python plugin_hooks.py --setup-environment
  python plugin_hooks.py --pre-uninstall
"""

import sys


def check_prerequisites() -> bool:
	"""Return True if prerequisites met. Print message and return False if not."""

	# C++ plugin only needs the precompiled binaries — no runtime prerequisite.
	return True


def setup_environment():
	"""Called after files are installed. Set env vars, etc."""

	# C++ plugin currently needs no environment setup.
	pass


def pre_uninstall():
	"""Called before plugin directory is removed. Clean up env vars, etc."""

	# C++ plugin currently has no environment cleanup needed.
	pass


if __name__ == "__main__":
	if len(sys.argv) < 2:
		print("Usage: python plugin_hooks.py --check-prerequisites|--setup-environment|--pre-uninstall")
		sys.exit(1)

	action = sys.argv[1]

	if action == '--check-prerequisites':
		ok = check_prerequisites()
		sys.exit(0 if ok else 1)

	elif action == '--setup-environment':
		setup_environment()
		sys.exit(0)

	elif action == '--pre-uninstall':
		pre_uninstall()
		sys.exit(0)

	else:
		print(f"Unknown action: {action}")
		sys.exit(1)
