# Movie Ticket Booking System

A modular console-based Movie Ticket Booking System developed in C++ for the
TCS-504 System Design assignment.

## Features
- List currently playing movies (5 movies across 3 screens, 2 shows each)
- List shows by movie
- Display seat layout and seat status
- Book **one or more seats in a single booking** — the whole request is
  all-or-nothing: if any requested seat is invalid, duplicate, or already
  booked, no seat is touched and no booking is created
- Seat-based pricing:
  - Silver: Rs.150
  - Gold: Rs.250
  - Platinum: Rs.400
- UPI, Card and Cash payment (UPI gets an automatic 5% instant discount)
- Failed payment does **not** confirm a booking — every seat in that
  request stays AVAILABLE
- Already-booked/duplicate seats are rejected with a clear message
- Ticket generation, with an extra note printed for cash payments, showing
  the Booking ID (e.g. `B1001`) and every seat in the booking
- **Cancel a booking by its Booking ID** (menu option 3) — this looks the
  booking up directly instead of re-picking a seat, and releases **every**
  seat that belongs to it back to AVAILABLE
- Invalid/garbage input never crashes or hangs the program

## How to Compile & Run

No header files are used, so compile the project through `main.cpp` only
(every other `.cpp` is pulled in via include guards):

```bash
g++ -std=c++17 main.cpp -o movie_booking
./movie_booking
```

On Windows:

```bash
g++ -std=c++17 main.cpp -o movie_booking.exe
movie_booking.exe
```

The compiled binary is intentionally **not** committed to the repository
(see `.gitignore`) — always build it fresh from source.

## Project Structure
One class per `.cpp` file, as required by the assignment:

| File | Responsibility |
|---|---|
| `Movie.cpp` | Movie details (encapsulation) |
| `Seat.cpp` | Physical seat + pricing |
| `Screen.cpp` | A screen and the seats it owns |
| `Cinema.cpp` | A cinema and the screens it owns |
| `Show.cpp` | A movie playing on a screen at a time, owns `ShowSeat`s |
| `ShowSeat.cpp` | Booking status of a seat for one particular show |
| `Customer.cpp` | Customer details |
| `Booking.cpp` | A confirmed/cancelled reservation |
| `Payment.cpp` | Abstract payment interface |
| `UpiPayment.cpp` / `CardPayment.cpp` / `CashPayment.cpp` | Concrete payment methods |
| `PriceCalculator.cpp` | Pricing logic only (with/without discount) |
| `TicketPrinter.cpp` | Ticket formatting/printing only |
| `BookingService.cpp` | Orchestrates the whole booking flow |
| `main.cpp` | Entry point + composition root (see DIP below) |

## OOP Concepts Demonstrated

- **Encapsulation** – every class keeps its data `private` and exposes only
  getters/behavior (`Movie`, `Seat`, `Customer`, `ShowSeat`, `Booking`, ...).
- **Abstraction** – `Payment` is a pure abstract class exposing only
  `pay(amount)`; callers never care how a specific method actually works.
- **Inheritance** – `UpiPayment`, `CardPayment`, `CashPayment` all derive
  from `Payment`.
- **`this` keyword** – used explicitly throughout the codebase, most
  visibly in constructors where the parameter name shadows the member name,
  e.g. in `Movie`:
  ```cpp
  Movie(string title, string language, int duration) {
      this->title = title;
      this->language = language;
      this->duration = duration;
  }
  ```
  It is also used to call one overload from another, e.g.
  `TicketPrinter::printTicket(booking)` calls
  `this->printTicket(booking, "")`.
- **Compile-time polymorphism (function overloading)** – demonstrated in
  two places:
  - `PriceCalculator::calculateTotal(seats)` and
    `PriceCalculator::calculateTotal(seats, discountPercent)` — the
    UPI discount path calls the two-argument overload, everything else
    calls the single-argument one. The compiler picks the overload purely
    from the argument list, at compile time.
  - `TicketPrinter::printTicket(booking)` and
    `printTicket(booking, note)` — cash payments print with an extra note.
- **Runtime polymorphism** – `BookingService` only ever holds a `Payment*`.
  `payment->pay(total)` dispatches to `UpiPayment::pay`, `CardPayment::pay`,
  or `CashPayment::pay` depending on the **actual object type**, decided at
  runtime through the virtual function table.
- **Static member** – `Booking::nextBookingId` is shared by every `Booking`
  instance to auto-generate unique booking IDs.
- **Composition** – `Cinema` owns `Screen`s, `Screen` owns `Seat`s, `Show`
  owns `ShowSeat`s (their lifetime is tied to the owner).
- **Aggregation** – `Show` merely *refers to* an existing `Movie*` and
  `Screen*`; it does not own or destroy them.
- **Association** – `Customer` interacts with `BookingService` without
  either owning the other.

## SOLID Principles

- **Single Responsibility Principle** – `PriceCalculator` only calculates
  prices, `TicketPrinter` only prints tickets, `BookingService` only
  orchestrates the flow; none of them mix these jobs.
- **Open/Closed Principle** – adding a new payment method (e.g.
  `WalletPayment`) only requires a new class derived from `Payment`; no
  existing class needs to change.
- **Liskov Substitution Principle** – any `Payment*` (Upi/Card/Cash) can be
  used wherever a `Payment*` is expected, and the program behaves correctly
  regardless of which concrete type is actually plugged in.
- **Dependency Inversion Principle** – this was a deliberate fix in this
  version. **`BookingService` no longer creates `UpiPayment`,
  `CardPayment`, or `CashPayment` directly** and does not even `#include`
  those files anymore. Instead:
  - `BookingService` depends only on the `Payment` abstraction, plus an
    injected factory: `function<Payment*(int, bool)> paymentFactory`.
  - The actual `new UpiPayment(...)`, `new CardPayment(...)`,
    `new CashPayment()` calls live in `main.cpp`'s `createPayment(...)`
    function — the **composition root** — which is the only file in the
    whole project that includes and knows about all three concrete payment
    classes.
  - This means the high-level module (`BookingService`) depends on an
    abstraction (`Payment`), and the low-level modules (concrete payments)
    are wired in from the outside, exactly as DIP requires.

## Manually Verified Behaviour
The following were run and confirmed by hand while finalizing this version:
- A normal multi-seat booking flow (successful UPI/Card/Cash payment)
  confirms the booking, marks **every** selected seat `BOOKED`, and prints a
  ticket listing the Booking ID and all seats.
- A **failed** payment (simulated "N" for UPI/Card) leaves the booking
  unconfirmed and **all** selected seats `AVAILABLE`.
- Attempting to book a seat that is already `BOOKED`, an invalid seat
  number, or a duplicate seat number within the same request is rejected
  with a message; no state changes (all-or-nothing).
- Cancelling by Booking ID (menu option 3) sets **every** seat in that
  booking back to `AVAILABLE`, after which those seats can be booked again.
  An unknown or already-cancelled Booking ID is reported without side
  effects.
- Garbage input (letters, out-of-range numbers, negative numbers, empty
  input, or the input stream ending unexpectedly) is always caught,
  reported to the user, and the program keeps running (or exits cleanly on
  end-of-input) — it never crashes and never hangs.

## Important Design Decision
A separate `Ticket` class was deliberately not created because the given
scope requires ticket *printing*, not an independently managed ticket
entity. `TicketPrinter` formats the ticket from the confirmed `Booking`.
